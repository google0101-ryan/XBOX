#include <emu/pci/dram_controller.h>

DramController::DramController(PCIBus* pci)
: PCIDevice(0x10de, 0x02a6, 0x05, 0x00)
{
	pci->RegisterPCIDevice(0, 0, 3, this);
}