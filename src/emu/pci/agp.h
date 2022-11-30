#pragma once

#include <emu/pci/pci.h>
#include <emu/io/IoBus.h>

// We put this class in this folder because it's not important enough
// To have its own folder

class AGP : public PCIDevice
{
public:
	AGP(PCIBus* bus);
};