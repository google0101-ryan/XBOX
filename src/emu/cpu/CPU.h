#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>

#include <emu/memory/Bus.h>
#include <emu/io/IoBus.h>

class CPU
{
private:
    const char* Reg32[8] =
    {
        "eax",
        "ecx",
        "edx",
        "ebx",
        "esp",
        "ebp",
        "esi",
        "edi"
    };

    const char* Reg16[8] =
    {
        "ax",
        "cx",
        "dx",
        "bx",
        "sp",
        "bp",
        "si",
        "di"
    };

    enum REGISTERS8
    {
        AL,
        CL,
        DL,
        BL,
        AH,
        CH,
        DH,
        BH
    };
    
    const char* Reg8[8] =
    {
        "al",
        "cl",
        "dl",
        "bl",
        "ah",
        "ch",
        "dh",
        "bh"
    };

    const char* SegNames[6] =
    {
        "es",
        "cs",
        "ss",
        "ds",
        "fs",
        "gs"
    };

    enum Segments
    {
        ES,
        CS,
        SS,
        DS,
        FS,
        GS
    };

    enum REGISTERS
    {
        EAX,
        ECX,
        EDX,
        EBX,
        ESP,
        EBP,
        ESI,
        EDI
    };

    union
    {
        uint32_t reg32;
        uint16_t reg16;
        struct
        {
            uint8_t reg8_lo;
            uint8_t reg8_hi;
        };
    } regs[8];

    uint32_t eip;

    struct
    {
        uint16_t selector;
        uint32_t base;
        uint32_t limit;
    } segs[6];

    uint32_t cr[6];

    bool isPE = false;
    bool a32 = false, o32 = false;
    bool csModified = false;

    uint32_t TranslateAddress(uint32_t addr, Segments seg)
    {
        if (cr[0] & (1 << 31))
        {
            printf("ERR: Unknown translation scheme!\n");
            exit(1);
        }
        else if (isPE)
        {
            uint32_t entry_addr = gdtr.base + ((segs[seg].base >> 3) * 8);
            uint32_t entry1 = bus->read<uint32_t>(entry_addr);
            uint32_t entry2 = bus->read<uint32_t>(entry_addr+4);

            uint32_t base_low = (entry1 >> 16);
            uint32_t base_mid = (entry2 & 0xFF);
            uint32_t base_high = (entry2 >> 24);

            uint64_t base = base_low | (base_mid << 16) | (base_high << 24);

            return base + addr;
        }

        if (!csModified && seg == CS)
            return segs[seg].base + addr;
        else
            return (segs[seg].base << 4) + addr;
    }

    struct
    {
        uint32_t base = 0;
        uint16_t limit = 0;
    } gdtr, idtr;

    struct
    {
        uint32_t base = 0;
        uint16_t limit = 0;
        uint16_t selector = 0;
    } ldtr, tr;

    union
    {
        uint8_t value;
        struct
        {
            uint8_t rm : 3;
            uint8_t reg : 3;
            uint8_t mod : 2;
        };
    } modrm;

    void LoadModRM()
	{
		modrm.value = bus->read<uint8_t>(TranslateAddress(eip++, CS));
	}

    Bus* bus;
	IoBus* iobus;
    Segments prefix = DS;

    using Opcode = std::function<void()>;

    std::unordered_map<uint8_t, Opcode> lookup, extended;

    uint8_t GetReg8(int reg)
    {
        if (reg < 4)
            return regs[reg].reg8_lo;
        else
            return regs[reg - 4].reg8_hi;
    }
    
    void SetReg8(int reg, uint8_t data)
    {
        if (reg < 4)
            regs[reg].reg8_lo = data;
        else
            regs[reg - 4].reg8_hi = data;
    }

    uint8_t disp8;
    uint16_t disp16;
    uint32_t disp32;
    bool isDisp8 = false;
    bool isDisp16 = false;
    bool isDisp32 = false;
    uint32_t GrabModRMAddress(std::string& disasm);
    uint32_t GetRM(std::string& disasm);
    uint8_t GetRM8(std::string& disasm);

	
    void SetRM(std::string& disasm, uint32_t val);

    enum Flags
    {
        CF = (1 << 0),
        RSV0 = (1 << 1),
        PF = (1 << 2),
        ZF = (1 << 6),
        SF = (1 << 7),
        OF = (1 << 11)
    };

    uint32_t flags;

    void SetFlag(Flags f, bool v)
    {
        if (v)
            flags |= f;
        else
            flags &= ~f;
    }

    bool GetFlag(Flags f)
    {
        return flags & f;
    }

    void or_al_imm8(); // 0x0C
    void xor_r_rm(); // 0x33
    void cmp_al_imm8(); // 0x3C
    void jne_rel8(); // 0x75
	void code_81(); // 0x81
	void cmp_rm_imm(); // 0x81 0x07
	void code_83(); // 0x83
	void add_rm_imm8(); // 0x83 0x00
    void mov_r8_rm8(); // 0x8A
    void mov_r_rm(); // 0x8B
    void mov_sreg_rm(); // 0x8E
    void mov_r8_imm8(); // 0xB0+r
    void mov_r_imm(); // 0xB8+r
	void jmp_rel32(); // 0xE9
    void jmp_ptr(); // 0xEA
    void jmp_rel8(); // 0xEB
	void out_dx_eax(); // 0xEF

    // 0x0F
    void code_0f01(); // 0x01
    void lgdt(); // 0x01 0x02
    void lidt(); // 0x01 0x03
    void mov_r32_cr(); // 0x20
    void mov_cr_r32(); // 0x22
public:
    CPU(Bus* bus, IoBus* iobus) : bus(bus), iobus(iobus) {Reset();}
    void Reset();

    void Clock();
    void Dump();
};