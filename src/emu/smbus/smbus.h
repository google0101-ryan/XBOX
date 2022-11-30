#pragma once

#include <emu/pci/pci.h>
#include <emu/io/IoBus.h>

class SMBUS : public PCIDevice, IoDev
{
private:
	uint32_t io_base = 0xC000;
	uint32_t io_base_2 = 0xC200;

	IoBus* iobus;

	void WriteByte(uint16_t port, uint8_t data) override;
	uint8_t ReadByte(uint16_t port) override;

	void write_config_dword(uint8_t offset, uint32_t data) override
	{
		printf("Writing 0x%02x to config space off 0x%02x\n", data, offset);
		*(uint32_t*)&config_space[offset] = data;
		if (offset >= 0x14 && offset < 0x18)
			RemapBAR(1);
		else if (offset >= 0x18 && offset < 0x1C)
			RemapBAR(2);
	}
public:
	SMBUS(PCIBus* pci, IoBus* io);

	void RemapBAR(int bar) override;
};