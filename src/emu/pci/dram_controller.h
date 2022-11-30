#pragma once

#include <emu/pci/pci.h>

class DramController : public PCIDevice
{
public:
	DramController(PCIBus* pci);
};