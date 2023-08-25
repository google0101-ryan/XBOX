#include "CPU.h"


void CPU::or_al_imm8()
{
    uint8_t imm8 = bus->read<uint8_t>(TranslateAddress(eip++, CS));

    regs[EAX].reg8_lo |= imm8;

    SetFlag(OF, 0);
    SetFlag(CF, 0);
    SetFlag(ZF, regs[EAX].reg8_lo == 0);
    SetFlag(SF, (regs[EAX].reg8_lo >> 7) & 1);

    printf("or al, 0x%02x\n", imm8);
}

void CPU::xor_r_rm()
{
    FetchModrm();
    std::string disasm;

    if (o32)
    {
        uint32_t rm32 = ReadModrm32(disasm);
        uint32_t r32 = regs[modrm.reg].reg32;

        uint32_t result = rm32 ^ r32;

        SetFlagsLogic32(result);

        regs[modrm.reg].reg32 = result;

        printf("xor %s, %s\n", Reg32[modrm.reg], disasm.c_str());
    }
    else
    {
        printf("TODO: xor16\n");
        exit(1);
    }
}

void CPU::mov_r8_rm8()
{
    FetchModrm();

    std::string disasm;
    SetReg8(modrm.reg, ReadModrm8(disasm));

    printf("mov %s, %s\n", Reg8[modrm.reg], disasm.c_str());
}

void CPU::mov_sreg_rm()
{
    FetchModrm();
    std::string disasm;

    segs[modrm.reg].selector = ReadModrm16(disasm);

    printf("mov %s, %s\n", SegNames[modrm.reg], disasm.c_str());
}

void CPU::mov_r8_imm8()
{
    uint8_t reg = bus->read<uint8_t>(TranslateAddress(eip-1, CS)) - 0xB0;
    uint8_t imm8 = bus->read<uint8_t>(TranslateAddress(eip++, CS));

    SetReg8(reg, imm8);

    printf("mov %s, 0x%02x\n", Reg8[reg], imm8);
}

void CPU::mov_r_imm()
{
    uint8_t reg = bus->read<uint8_t>(TranslateAddress(eip-1, CS)) - 0xB8;

    if (o32)
    {
        uint32_t imm32 = bus->read<uint32_t>(TranslateAddress(eip, CS));
        eip += 4;

        regs[reg].reg32 = imm32;

        printf("mov %s, 0x%08x\n", Reg32[reg], imm32);
    }
    else
    {
        printf("TODO: mov r16, imm16\n");
        exit(1);
    }
}

void CPU::jmp_ptr()
{
    uint32_t addr = bus->read<uint32_t>(TranslateAddress(eip, CS));
    eip += 4;
    uint16_t segment = bus->read<uint32_t>(TranslateAddress(eip, CS));
    eip += 2;

    eip = addr;
    segs[CS].base = segment;

    printf("jmp 0x%02x:0x%08x\n", segment, addr);

    if (cr[0] & 1)
        isPE = true;
}

void CPU::jmp_rel8()
{
    int8_t rel8 = bus->read<uint8_t>(TranslateAddress(eip++, CS));

    eip += rel8;

    printf("jmp 0x%08x\n", TranslateAddress(eip, CS));
}

void CPU::lgdt()
{
    std::string disasm = "";
    uint32_t addr = GrabModRMAddress(disasm);
    
    gdtr.limit = bus->read<uint16_t>(TranslateAddress(addr, prefix));
    gdtr.base = bus->read<uint32_t>(TranslateAddress(addr+2, prefix));

    printf("lgdt [%s]\n", disasm.c_str());
}

void CPU::lidt()
{
    std::string disasm = "";
    uint32_t addr = GrabModRMAddress(disasm);
    
    idtr.limit = bus->read<uint16_t>(TranslateAddress(addr, prefix));
    idtr.base = bus->read<uint32_t>(TranslateAddress(addr+2, prefix));

    printf("lidt %s\n", disasm.c_str());
}

void CPU::mov_r32_cr()
{
    modrm.value = bus->read<uint8_t>(TranslateAddress(eip++, CS));

    regs[modrm.rm].reg32 = cr[modrm.reg];

    printf("mov %s, cr%d\n", Reg32[modrm.rm], modrm.reg);
}

void CPU::mov_cr_r32()
{
    modrm.value = bus->read<uint8_t>(TranslateAddress(eip++, CS));

    cr[modrm.reg] = regs[modrm.rm].reg32;

    printf("mov cr%d, %s\n", modrm.reg,  Reg32[modrm.rm]);
}

void CPU::code_0f01()
{
    FetchModrm();

    switch (modrm.reg)
    {
    case 0x02:
        lgdt();
        break;
    case 0x03:
        lidt();
        break;
    default:
        printf("Unknown opcode 0x0f 0x01 0x%01x\n", modrm.reg);
        exit(1);
    }
}