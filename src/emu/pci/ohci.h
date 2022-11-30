#pragma once

#include <emu/pci/pci.h>
#include <emu/memory/Bus.h>

class OHCI : public PCIDevice, MemoryDevice
{
private:
	Bus* bus;

	uint32_t start_address = 0xfed00000;
	static constexpr uint32_t USB_MMIO_SIZE = 4096;
	
	uint8_t mem_ReadByte(uint32_t ) override {return 0;}
    void mem_WriteByte(uint32_t , uint8_t) override {}
    uint16_t mem_ReadWord(uint32_t ) override {return 0;}
    void mem_WriteWord(uint32_t , uint16_t) override {}
    uint32_t mem_ReadDoubleWord(uint32_t ) override {return 0;}
    void mem_WriteDoubleWord(uint32_t , uint32_t) override {}
public:
	OHCI(PCIBus* pci, Bus* bus, int num);

	uint32_t GetStart() override {return start_address;}
	uint32_t GetEnd() override {return start_address + USB_MMIO_SIZE;}

	void RemapBAR(int bar) override;
};