#include <emu/cpu/CPU.h>
#include <cstring>
#include <sstream>
#include <cmath>
#include <map>

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

	lookup[0x02] = std::bind(&CPU::add_r8_rm8, this);
	lookup[0x03] = std::bind(&CPU::add_r_rm, this);
	lookup[0x05] = std::bind(&CPU::add_eax_imm, this);
	lookup[0x0B] = std::bind(&CPU::or_r_rm, this);
    lookup[0x0C] = std::bind(&CPU::or_al_imm8, this);
	lookup[0x23] = std::bind(&CPU::and_r_rm, this);
	lookup[0x25] = std::bind(&CPU::and_eax_imm, this);
	lookup[0x32] = std::bind(&CPU::xor_r8_rm8, this);
    lookup[0x33] = std::bind(&CPU::xor_r_rm, this);
	lookup[0x3B] = std::bind(&CPU::cmp_r_rm, this);
    lookup[0x3C] = std::bind(&CPU::cmp_al_imm8, this);
	for (int i = 0; i < 8; i++)
	{
		lookup[0x40+i] = std::bind(&CPU::inc_r, this);
	}
	for (int i = 0; i < 8; i++)
	{
		lookup[0x48+i] = std::bind(&CPU::dec_r, this);
	}
	lookup[0x72] = std::bind(&CPU::jb_rel8, this);
	lookup[0x74] = std::bind(&CPU::je_rel8, this);
    lookup[0x75] = std::bind(&CPU::jne_rel8, this);
	lookup[0x76] = std::bind(&CPU::jbe_rel8, this);
	lookup[0x80] = std::bind(&CPU::code_80, this);
	lookup[0x81] = std::bind(&CPU::code_81, this);
	lookup[0x83] = std::bind(&CPU::code_83, this);
	lookup[0x88] = std::bind(&CPU::mov_rm8_r8, this);
	lookup[0x89] = std::bind(&CPU::mov_rm_r, this);
    lookup[0x8A] = std::bind(&CPU::mov_r8_rm8, this);
    lookup[0x8B] = std::bind(&CPU::mov_r_rm, this);
    lookup[0x8E] = std::bind(&CPU::mov_sreg_rm, this);
    for (int i = 0; i < 8; i++)
    {
        lookup[0xB0 + i] = std::bind(&CPU::mov_r8_imm8, this);
    }
    for (int i = 0; i < 8; i++)
    {
        lookup[0xB8 + i] = std::bind(&CPU::mov_r_imm, this);
    }
	lookup[0xE9] = std::bind(&CPU::jmp_rel32, this);
    lookup[0xEA] = std::bind(&CPU::jmp_ptr, this);
    lookup[0xEB] = std::bind(&CPU::jmp_rel8, this);
	lookup[0xEC] = std::bind(&CPU::in_al_dx, this);
	lookup[0xED] = std::bind(&CPU::in_eax_dx, this);
	lookup[0xEE] = std::bind(&CPU::out_dx_al, this);
	lookup[0xEF] = std::bind(&CPU::out_dx_eax, this);
	lookup[0xFE] = std::bind(&CPU::inc_dec_rm8, this);

    extended[0x01] = std::bind(&CPU::code_0f01, this);
    extended[0x20] = std::bind(&CPU::mov_r32_cr, this);
    extended[0x22] = std::bind(&CPU::mov_cr_r32, this);
	extended[0x30] = std::bind(&CPU::wrmsr, this);
	extended[0xB6] = std::bind(&CPU::movzx_r_rm, this);

    SetFlag(RSV0, 1);
}

void CPU::Clock()
{
    o32 = a32 = isPE;
    prefix = DS;
    isDisp8 = isDisp16 = isDisp32 = false;

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
	const char* m = "0123456789ABCDEF";

	std::string res = "";

	if (!num)
		return "0";
	
	if (num > 0)
	{
		while (num)
		{
			res = m[num % 16] + res;
			num /= 16;
		}
		res = "0x" + res;
	}
	else
	{
		uint32_t n = -num;
		while (n)
		{
			res = m[n % 16] + res;
			n /= 16;
		}
		res = "-0x" + res;
	}

	return res;
}

uint32_t CPU::GrabModRMAddress(std::string& disasm)
{
    if (!a32)
    {
        switch (modrm.mod)
        {
        case 0x00:
            switch (modrm.rm)
            {
            case 0x06:
				if (!isDisp16)
				{
					disp16 = bus->read<int16_t>(TranslateAddress(eip, CS));
					eip += 2;
				}
                disasm = "[" + convert_int(TranslateAddress(disp16, prefix)) + "]";
                isDisp16 = true;
                return disp16;
            }
        }
    }
    else
    {
		if (modrm.rm == 4)
			sib.value = bus->read<uint8_t>(TranslateAddress(eip++, CS));

        switch (modrm.mod)
        {
        case 0:
            switch (modrm.rm)
            {
			case 2:
				disasm = "[edx]";
				return regs[EDX].reg32;
			case 3:
				disasm = "[ebx]";
				return regs[EBX].reg32;
			case 4:
				return GrabSIB(disasm);
            case 6:
                disasm = "[esi]";
                return regs[ESI].reg32;
			case 7:
				disasm = "[edi]";
				return regs[EDI].reg32;
            }
            break;
        case 1:
        {
			if (!isDisp8)
           		disp8 = bus->read<int8_t>(TranslateAddress(eip++, CS));
            isDisp8 = true;
            switch (modrm.rm)
            {
			case 4:
				return GrabSIB(disasm) + disp8;
            case 6:
                disasm = "[esi + " + convert_int(disp8) + "]";
                return regs[ESI].reg32 + disp8;
            }
			break;
        }
		case 2:
		{
			if (!isDisp32)
			{
				disp32 = bus->read<int32_t>(TranslateAddress(eip, CS));
				eip += 4;
			}
			isDisp32 = true;
			switch (modrm.rm)
			{
			case 6:
                disasm = "[esi + " + convert_int(disp32) + "]";
                return regs[ESI].reg32 + disp32;
			case 7:
                disasm = "[edi + " + convert_int(disp32) + "]";
                return regs[EDI].reg32 + disp32;
			}
		}
        }
    }

    printf("Unknown mod %d, reg %d, rm %d\n", modrm.mod, modrm.reg, modrm.rm);
    exit(1);
}

uint32_t CPU::GrabSIB(std::string& disasm)
{
	uint32_t base_val = regs[sib.base].reg32;
	uint32_t scale = pow(2, sib.scale);
	uint32_t index_val = regs[sib.index].reg32;

	if (modrm.mod == 0)
	{
		if (sib.base == 5)
		{
			disp32 = bus->read<uint32_t>(TranslateAddress(eip, CS));
			eip += 4;
			isDisp32 = true;
		}

		if (sib.index == 4 && sib.base == 5)
		{
			disasm = "[" + convert_int(disp32) + "]";
			return disp32;
		}
		if (sib.index == 4)
		{
			disasm = "[" + std::string(Reg32[sib.base]) + "]";
			return base_val;
		}
		if (sib.base == 5)
		{
			disasm = "[" + std::string(Reg32[sib.index]) + " * " + convert_int(scale) + " + " + convert_int(disp32) + "]";
			return (index_val * scale) + disp32;
		}
		disasm = "[" + std::string(Reg32[sib.index]) + " * " + convert_int(scale) + " + " + std::string(Reg32[sib.base]) + "]";
		return index_val * scale + base_val;
	}

	if (modrm.mod == 1)
	{
		if (sib.index == 4)
		{
			disasm = "[" + std::string(Reg32[sib.base]) + " + " + convert_int(disp8) + "]";
		}
		disasm = "[" + std::string(Reg32[sib.index]) + " * " + convert_int(scale) + " + " + std::string(Reg32[sib.base]) + " + " + convert_int(disp8) + "]";
		return base_val + (index_val * scale) + disp8;
	}

	printf("Unknown scale %d, index %d, base %d\n", sib.scale, sib.index, sib.base);
	exit(1);
}

uint32_t CPU::GetRM(std::string& disasm)
{
    if (modrm.mod == 3)
    {
        disasm = Reg32[modrm.rm];
        return regs[modrm.rm].reg32;
    }

    return bus->read<uint32_t>(TranslateAddress(GrabModRMAddress(disasm), prefix));
}

void CPU::SetRM(std::string& disasm, uint32_t val)
{
    if (modrm.mod == 3)
    {
        disasm = Reg32[modrm.rm];
        regs[modrm.rm].reg32 = val;
		return;
    }

	bus->write<uint32_t>(TranslateAddress(GrabModRMAddress(disasm), prefix), val);
}

void CPU::SetRM8(std::string& disasm, uint8_t val)
{
    if (modrm.mod == 3)
    {
        disasm = Reg8[modrm.rm];
		SetReg8(modrm.rm, val);
		return;
    }

	bus->write<uint8_t>(TranslateAddress(GrabModRMAddress(disasm), prefix), val);
}

uint8_t CPU::GetRM8(std::string& disasm)
{
    if (modrm.mod == 3)
    {
        disasm = Reg8[modrm.rm];
        return GetReg8(modrm.rm);
    }

    return bus->read<uint16_t>(TranslateAddress(GrabModRMAddress(disasm), prefix));
}