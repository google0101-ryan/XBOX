#pragma once

#include <cstdint>
#include <emu/memory/Rom.h>
#include <emu/memory/Bus.h>
#include <emu/io/IoBus.h>
#include <emu/pci/pci.h>
#include <emu/pci/isa.h>
#include <emu/cpu/CPU.h>

class System
{
private:
    Rom* rom;
    Bus* bus;
	IoBus* iobus;
	PCIBus* pci;
	ISA* isa;
    CPU* cpu;
public:
    System();

    void Clock();
    void Dump();
};