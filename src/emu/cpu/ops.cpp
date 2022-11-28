#include <emu/cpu/CPU.h>

void CPU::or_al_imm8()
{
    uint8_t imm8 = bus->read<uint8_t>(TranslateAddress(eip++, CS));

    uint8_t result = regs[EAX].reg8_lo | imm8;
    regs[EAX].reg8_lo = result;

    SetFlag(OF, 0);
    SetFlag(CF, 0);
    SetFlag(ZF, result == 0);
    SetFlag(SF, result & (1 << 7));

    printf("or al, 0x%02x\n", imm8);
}

void CPU::xor_r_rm()
{
    std::string disasm;
    modrm.value = bus->read<uint8_t>(TranslateAddress(eip++, CS));

    uint32_t rm32 = GetRM(disasm);
    uint32_t reg32 = regs[modrm.reg].reg32;

    uint32_t result = rm32 ^ reg32;

    regs[modrm.reg].reg32 = result;

    SetFlag(OF, 0);
    SetFlag(CF, 0);
    SetFlag(ZF, result == 0);
    SetFlag(SF, result & (1 << 31));

    printf("xor %s, %s\n", Reg32[modrm.reg], disasm.c_str());
}

void CPU::cmp_al_imm8()
{
    uint8_t value = bus->read<uint8_t>(TranslateAddress(eip++, CS));
    uint8_t al = GetReg8(AL);

    uint16_t result = (uint16_t)al - (uint16_t)value;

    SetFlag(CF, result >> 8);
    SetFlag(ZF, result == 0);
    SetFlag(SF, (result >> 7) & 1);

    printf("cmp al, 0x%02x\n", value);
}

void CPU::jne_rel8()
{
    int8_t rel8 = bus->read<int8_t>(TranslateAddress(eip++, CS));

    printf("jne 0x%08x\n", eip + rel8);

    if (!GetFlag(ZF))
    {
        eip += rel8;
    }
}

void CPU::code_81()
{
	LoadModRM();

	switch (modrm.reg)
	{
	case 0x07:
		cmp_rm_imm();
		break;
	default:
        printf("Unknown opcode 0x81 0x%01x\n", modrm.reg);
        exit(1);
	}
}

void CPU::cmp_rm_imm()
{
	std::string disasm;
	uint32_t rm32 = GetRM(disasm);
	uint32_t imm32 = bus->read<uint32_t>(TranslateAddress(eip, CS));
	eip += 4;

	uint64_t result = rm32 - imm32;

	SetFlag(CF, result >> 32);
	SetFlag(ZF, (result & 0xFFFFFFFF) == 0);
	SetFlag(SF, result & (1 << 31));

	printf("cmp %s, 0x%08x\n", disasm.c_str(), imm32);
}

void CPU::code_83()
{
	LoadModRM();

	switch (modrm.reg)
	{
	case 0x00:
		add_rm_imm8();
		break;
	default:
        printf("Unknown opcode 0x83 0x%01x\n", modrm.reg);
        exit(1);
	}
}

void CPU::add_rm_imm8()
{
	std::string disasm;
	uint32_t rm = GetRM(disasm);
	uint8_t imm8 = bus->read<uint8_t>(TranslateAddress(eip++, CS));

	uint64_t result = (uint64_t)rm + (uint64_t)imm8;

	SetFlag(CF, result >> 32);
	SetFlag(ZF, (result & 0xFFFFFFFF) == 0);
	SetFlag(SF, result & (1 << 31));

	printf("add %s, 0x%02x\n", disasm.c_str(), imm8);

	SetRM(disasm, result & 0xFFFFFFFF);
}

void CPU::mov_r8_rm8()
{
    modrm.value = bus->read<uint8_t>(TranslateAddress(eip++, CS));

    std::string disasm;
    SetReg8(modrm.reg, GetRM8(disasm));

    printf("mov %s, %s\n", Reg8[modrm.reg], disasm.c_str());
}

void CPU::mov_r_rm()
{
    modrm.value = bus->read<uint8_t>(TranslateAddress(eip++, CS));
    if (o32)
    {
        std::string disasm;
        regs[modrm.reg].reg32 = GetRM(disasm);

        printf("mov %s, %s\n", Reg32[modrm.reg], disasm.c_str());
    }
    else
    {
        printf("mov r16, rm16 undefined\n");
        exit(1);
    }
}

void CPU::mov_sreg_rm()
{
    modrm.value = bus->read<uint8_t>(TranslateAddress(eip++, CS));

    std::string disasm;
    segs[modrm.reg].base = GetRM(disasm);

    printf("mov %s, %s\n", SegNames[modrm.reg], disasm.c_str());
}

void CPU::mov_r8_imm8()
{
    uint8_t reg = bus->read<uint8_t>(TranslateAddress(eip-1, CS)) - 0xB0;
    uint8_t imm = bus->read<uint8_t>(TranslateAddress(eip++, CS));

    SetReg8(reg, imm);

    printf("mov %s, 0x%02x\n", Reg8[reg], imm);
}

void CPU::mov_r_imm()
{
    uint8_t reg = bus->read<uint8_t>(TranslateAddress(eip-1, CS)) - 0xB8;
    
    if (o32)
    {
        uint32_t imm = bus->read<uint32_t>(TranslateAddress(eip, CS));
        eip += 4;

        regs[reg].reg32 = imm;

        printf("mov %s, 0x%08x\n", Reg32[reg], imm);
    }
    else
    {
        uint16_t imm = bus->read<uint16_t>(TranslateAddress(eip, CS));
        eip += 2;

        regs[reg].reg16 = imm;

        printf("mov %s, 0x%08x\n", Reg16[reg], imm);
    }
}

void CPU::jmp_rel32()
{
    int32_t rel32 = bus->read<uint32_t>(TranslateAddress(eip, CS));
	eip += 4;

    eip += rel32;

    printf("jmp 0x%08x\n", TranslateAddress(eip, CS));
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

void CPU::out_dx_eax()
{
	uint16_t port = regs[EDX].reg16;
	uint32_t data = regs[EAX].reg32;

	printf("out dx, eax\n");

	iobus->write<uint32_t>(port, data);
}

void CPU::lgdt()
{
    std::string disasm = "";
    uint32_t addr = GrabModRMAddress(disasm);
    
    gdtr.limit = bus->read<uint16_t>(TranslateAddress(addr, prefix));
    gdtr.base = bus->read<uint32_t>(TranslateAddress(addr+2, prefix));

    printf("lgdt %s\n", disasm.c_str());
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
    modrm.value = bus->read<uint8_t>(TranslateAddress(eip++, CS));

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