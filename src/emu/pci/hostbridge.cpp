#include <emu/pci/hostbridge.h>

HostBridge::HostBridge(PCIBus* pci, Bus* bus)
: PCIDevice(0x10de, 0x02a5, 0x06, 0x00),
MemoryDevice("Host Bridge Adapter")
{
	pci->RegisterPCIDevice(0, 0, 0, this);
	bus->AttachDevice(this);
}

void HostBridge::RemapBAR(int bar)
{
	uint8_t config_space_off = bar * 4 + 0x10;

	uint32_t addr = read_config_dword(config_space_off);

	bool is_io = addr & 1;
	if (is_io)
		addr &= 0xFFFFFFFC;
	else
		addr &= 0xFFFFFFF0;

	printf("Remapping Host Bridge bus %s to 0x%08x\n", is_io ? "I/O" : "memory", addr);

	if (!is_io)
		base_addr = addr;
}