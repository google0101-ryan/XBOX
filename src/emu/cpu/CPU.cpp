#include <emu/cpu/CPU.h>
#include <cstring>
#include <sstream>
#include <cmath>
#include <map>
#include "CPU.h"

void CPU::Reset()
{
    std::memset(regs, 0, sizeof(regs));

    eip = 0xFFF0;

    segs[CS].base = 0xFFFF0000;
    segs[CS].limit = 0xFFFF;
    segs[CS].selector = 0xF000;

    segs[ES].base = 0;
    segs[ES].limit = 0xFFFF;
    segs[ES].selector = 0;

    for (int i = SS; i < GS; i++)
    {
        segs[i].base = 0;
        segs[i].limit = 0xFFFF;
        segs[i].selector = 0;
    }

    std::memset(cr, 0, sizeof(cr));

    cr[0] = 0x60000010;

    lookup[0x0C] = std::bind(&CPU::or_al_imm8, this);
    lookup[0x33] = std::bind(&CPU::xor_r_rm, this);
    lookup[0x8a] = std::bind(&CPU::mov_r8_rm8, this);
    lookup[0x8e] = std::bind(&CPU::mov_sreg_rm, this);
    for (int i = 0; i < 8; i++)
    {
        lookup[0xB0+i] = std::bind(&CPU::mov_r8_imm8, this);
    }
    for (int i = 0; i < 8; i++)
    {
        lookup[0xB8+i] = std::bind(&CPU::mov_r_imm, this);
    }
    lookup[0xEA] = std::bind(&CPU::jmp_ptr, this);
    lookup[0xEB] = std::bind(&CPU::jmp_rel8, this);

    extended[0x01] = std::bind(&CPU::code_0f01, this);
    extended[0x20] = std::bind(&CPU::mov_r32_cr, this);
    extended[0x22] = std::bind(&CPU::mov_cr_r32, this);

    SetFlag(RSV0, 1);
}

void CPU::Clock()
{
    o32 = a32 = isPE;
    prefix = DS;

	if (TranslateAddress(eip, CS) == 0xffffff81)
		printf("0x%08x\n", bus->read<uint32_t>(0x95fe4));

    uint8_t opcode = bus->read<uint8_t>(TranslateAddress(eip++, CS));

    bool isPrefix = true;

    std::unordered_map<uint8_t, Opcode> tbl = lookup;
    bool isExtended = false;

    while (isPrefix)
    {
        switch (opcode)
        {
        case 0x0F:
            tbl = extended;
            isExtended = true;
            break;
        case 0x2E:
            prefix = CS;
            break;
        case 0x66:
            o32 = !o32;
            printf("66 ");
            break;
        default:
            isPrefix = false;
        }

        if (isPrefix)
            opcode = bus->read<uint8_t>(TranslateAddress(eip++, CS));
    }
    
    Opcode op = tbl[opcode];

    if (!op)
    {    
        printf("Unknown opcode%s 0x%02x\n", isExtended ? " 0x0f" : "", opcode);
        exit(1);
    }

    op();
}

void CPU::Dump()
{
    for (int i = 0; i < 8; i++)
        printf("%s:\t->\t0x%08x\n", Reg32[i], regs[i].reg32);
    printf("eip\t->\t0x%08x\n", eip);
    printf("eflags\t->\t[%s%s%s%s%s]\n", GetFlag(ZF) ? "z" : ".", GetFlag(SF) ? "s" : ".", GetFlag(CF) ? "c" : ".", GetFlag(DF) ? "d" : ".", GetFlag(IF) ? "i" : ".");
}

std::string convert_int(int num)
{
    std::stringstream ss;
    ss << "0x" << std::hex << num;
    return ss.str();
}

void CPU::FetchModrm()
{
    modrm.value = bus->read<uint8_t>(TranslateAddress(eip++, CS));
    printf("0x%02x ", modrm.value);

    if (a32)
    {
        switch (modrm.mod)
        {
        case 0:
            switch (modrm.rm)
            {
            case 5:
                disp32 = bus->read<int32_t>(TranslateAddress(eip, CS));
                eip += 4;
                break;
            case 4:
                printf("SIB!\n");
                exit(1);
            default:
                break;
            }
            break;
        case 3: // Reg, no further data necessary
            break;
        default:
            printf("Unknown fetch32 modrm.mod=%d\n", modrm.mod);
            exit(1);
        }
    }
    else
    {
        switch (modrm.mod)
        {
        case 0:
            switch (modrm.rm)
            {
            case 6:
                disp16 = bus->read<int16_t>(TranslateAddress(eip, CS));
                eip += 2;
                break;
            default:
                printf("Unknown modrm.mod16=0 modrm.rm16=%d\n", modrm.rm);
                exit(1);
            }
            break;
        default:
            printf("Unknown modrm.mod16=%d\n", modrm.mod);
            exit(1);
        }
    }
}

uint32_t CPU::GrabModRMAddress(std::string &disasm)
{
    if (a32)
    {
        switch (modrm.mod)
        {
        case 0:
        {
            switch (modrm.rm)
            {
            case 4:
            {
                printf("SIB!\n");
                exit(1);
            }
            case 5:
            {
                disasm = convert_int(disp32);
                return disp32;
            }
            default:
                disasm = std::string("[") + Reg32[modrm.rm] + "]";
                return regs[modrm.rm].reg32;
            }
            break;
        }
        default:
            printf("Unknown modrm.mod32 %d\n", modrm.mod);
            exit(1);
        }
    }
    else
    {
        switch (modrm.mod)
        {
        case 0:
        {
            switch (modrm.rm)
            {
            case 6:
            {
                disasm = convert_int(disp16);
                return disp16;
            }
            default:
                printf("Unknown modrm.mod=0, modrm.rm16=%d\n", modrm.rm);
                exit(1);
            }
            break;
        }
        default:
            printf("Unknown modrm.mod16 %d\n", modrm.mod);
            exit(1);
        }
    }
}

uint32_t CPU::ReadModrm32(std::string &disasm)
{
    if (modrm.mod != 3)
        return bus->read<uint32_t>(TranslateAddress(GrabModRMAddress(disasm), prefix));
    else
    {
        disasm = Reg32[modrm.rm];
        return regs[modrm.rm].reg32;
    }
}

uint16_t CPU::ReadModrm16(std::string &disasm)
{
    if (modrm.mod != 3)
        return bus->read<uint16_t>(TranslateAddress(GrabModRMAddress(disasm), prefix));
    else
    {
        disasm = Reg16[modrm.rm];
        return regs[modrm.rm].reg16;
    }
}

uint8_t CPU::ReadModrm8(std::string &disasm)
{
    if (modrm.mod != 3)
        return bus->read<uint8_t>(TranslateAddress(GrabModRMAddress(disasm), prefix));
    else
    {
        disasm = Reg8[modrm.rm];
        return GetReg8(modrm.rm);
    }
}

void CPU::SetFlagsLogic32(uint32_t result)
{
    SetFlag(CF, 0);
    SetFlag(OF, 0);
    SetFlag(ZF, result == 0);
    SetFlag(SF, (result >> 31) & 1);
}
