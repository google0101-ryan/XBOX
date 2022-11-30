#pragma once

#include <cstdint>
#include <emu/memory/Rom.h>
#include <emu/memory/Bus.h>
#include <emu/io/IoBus.h>
#include <emu/pci/pci.h>
#include <emu/pci/isa.h>
#include <emu/pci/agp.h>
#include <emu/nv2a/nv2a.h>
#include <emu/pci/hostbridge.h>
#include <emu/pci/dram_controller.h>
#include <emu/pci/ohci.h>
#include <emu/smbus/smbus.h>
#include <emu/cpu/CPU.h>

class System
{
private:
    Rom* rom;
    Bus* bus;
	IoBus* iobus;
	PCIBus* pci;
	ISA* isa;
	AGP* agp;
	HostBridge* hostbridge;
	NV2A* nv2a;
	SMBUS* smbus;
	DramController* dram;
	OHCI* usb1, *usb2;
    CPU* cpu;
public:
    System();

    void Clock();
    void Dump();
};