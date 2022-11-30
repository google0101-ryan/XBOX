#include <emu/pci/ohci.h>

OHCI::OHCI(PCIBus* pci, Bus* bus, int num)
: PCIDevice(0x10de, 0x01c2, 0x0c, 0x03),
MemoryDevice("OHCI USB hub")
{
	uint32_t base = (num == 0 ? 0xfed00000 : 0xfed08000);
	int dev_num = (num == 0 ? 2 : 3);

	write_config_dword(0x04, 0x00b00006);
	write_config_dword(0x08, 0x0c0310b1);
	write_config_dword(0x10, base);
	write_config_dword(0x34, 0x44);
	write_config_dword(0x3C, 0x01030101);
	write_config_dword(0x44, 0xda020001);
	write_config_dword(0x4C, 0x2);
	write_config_dword(0x50, 0xf);

	bus->AttachDevice(this);
	pci->RegisterPCIDevice(0, dev_num, 0, this);
}

void OHCI::RemapBAR(int bar)
{
	uint32_t address = read_config_dword(0x10) & ~1; // Just in case we somehow became IO
	start_address = address;
}