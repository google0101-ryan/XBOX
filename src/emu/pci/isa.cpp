#include <emu/pci/isa.h>

ISA::ISA(PCIBus* bus)
: PCIDevice(0x10de, 0x01b2, 0x06, 0x01)
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
} 