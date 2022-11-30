#pragma once

#include <emu/pci/pci.h>
#include <emu/memory/Bus.h>

// Engines
#include <emu/nv2a/engines/pmc.h>
#include <emu/nv2a/engines/pbus.h>
#include <emu/nv2a/engines/pfb.h>
#include <emu/nv2a/engines/pramdac.h>

class NV2A : public PCIDevice, MemoryDevice
{
private:
	int start_address = 0xfd000000;
	int mem_len = 0x3000000;

	uint8_t mem_ReadByte(uint32_t ) override;
    void mem_WriteByte(uint32_t , uint8_t) override;
    uint16_t mem_ReadWord(uint32_t ) override;
    void mem_WriteWord(uint32_t , uint16_t) override;
    uint32_t mem_ReadDoubleWord(uint32_t ) override;
    void mem_WriteDoubleWord(uint32_t , uint32_t) override;

	NV2A_ENGINES::PMC* pmc;
	NV2A_ENGINES::PBUS* pbus;
	NV2A_ENGINES::PFB* pfb;
	NV2A_ENGINES::PRAMDAC* pramdac;
public:
	NV2A(PCIBus* pci, Bus* bus);

	void RemapBAR(int bar) override;

	uint32_t GetStart() override {return start_address;}
	uint32_t GetEnd() override {return start_address+mem_len-1;}
};