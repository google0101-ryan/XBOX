#include <emu/smbus/smbus.h>

SMBUS::SMBUS(PCIBus* pci, IoBus* bus)
: PCIDevice(0x10de, 0x01b4, 0x0c, 0x05),
IoDev("System Management Bus"),
iobus(bus)
{
	pci->RegisterPCIDevice(0, 1, 1, this);

	write_config_dword(0x14, 0xC001);
	write_config_dword(0x18, 0xC201);
}

void SMBUS::RemapBAR(int bar)
{
	uint32_t base_address = read_config_dword(bar * 4 + 0x10) & ~1;

	uint32_t io_to_unmap = 0;

	switch (bar) // Figure out what we're unmapping
	{
	case 1:
		io_to_unmap = io_base;
		break;
	case 2:
		io_to_unmap = io_base_2;
		break;
	}

	for (int i = 0; i < (bar == 1 ? 16 : 32); i++)
	{
		iobus->UnregisterDevice(io_to_unmap + i);
		iobus->RegisterDevice(this, base_address + i);
	}
}

void SMBUS::WriteByte(uint16_t port, uint8_t data)
{
	printf("Write 0x%02x to unknown SMBUS addr 0x%04x\n", data, port);
}

uint8_t SMBUS::ReadByte(uint16_t port)
{
	printf("Read from unknown SMBUS addr 0x%04x\n", port);
	if (port == 0xC000)
		return (1 << 4);
	else
		return 0;
}