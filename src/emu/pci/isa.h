#pragma once

#include <emu/pci/pci.h>

// We put this class in this folder because it's not important enough
// To have its own folder

class ISA : public PCIDevice
{
public:
	ISA(PCIBus* bus);

	void RemapBAR(int bar) override;
};