#pragma once

#include <emu/pci/pci.h>
#include <emu/memory/Bus.h>

class HostBridge : public PCIDevice, MemoryDevice
{
private:
	uint32_t base_addr = 0x40000008;
	uint32_t size = 0x40000000;

	uint8_t mem_ReadByte(uint32_t ) override {return 0;}
    void mem_WriteByte(uint32_t , uint8_t) override {}
    uint16_t mem_ReadWord(uint32_t ) override {return 0;}
    void mem_WriteWord(uint32_t , uint16_t) override {}
    uint32_t mem_ReadDoubleWord(uint32_t ) override {return 0;}
    void mem_WriteDoubleWord(uint32_t , uint32_t) override {}
public:
	HostBridge(PCIBus* pci, Bus* bus);

	void RemapBAR(int bar) override;

	uint32_t GetStart() override {return base_addr;}
    uint32_t GetEnd() override {return base_addr + size;}
};