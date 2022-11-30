#include <emu/pci/isa.h>

ISA::ISA(PCIBus* bus, IoBus* iobus)
: 
PCIDevice(0x10de, 0x01b2, 0x06, 0x01),
IoDev("Industry Standard Architecture Bridge"),
iobus(iobus)
{
	bus->RegisterPCIDevice(0, 1, 0, this);

	write_config_dword(0x10, 0x8001);
}

void ISA::RemapBAR(int bar)
{
	uint8_t config_space_off = bar * 4 + 0x10;

	uint32_t addr = read_config_dword(config_space_off);

	bool is_io = addr & 1;
	if (is_io)
		addr &= 0xFFFFFFFC;
	else
		addr &= 0xFFFFFFF0;

	printf("Remapping ISA bus %s to 0x%08x\n", is_io ? "I/O" : "memory", addr);

	if (is_io)
	{	
		for (int i = 0; i < 256; i++)
		{
			iobus->UnregisterDevice(cur_io_addr + i);
			iobus->RegisterDevice(this, addr + i);
		}
	}

	cur_io_addr = addr;
} 