#include <emu/System.h>

System::System()
{
    bus = new Bus();
    
    if (!bus)
    {
        printf("ERR: Could not initialize system\n");
        exit(1);
    }

    rom = new Rom("mcpx_1.0.bin");

    if (!bus)
    {
        printf("ERR: Could not initialize system\n");
        exit(1);
    }

    bus->AttachDevice(rom);

	iobus = new IoBus();

	if (!iobus)
    {
        printf("ERR: Could not create I/O bus\n");
        exit(1);
    }

	pci = new PCIBus(iobus);

	isa = new ISA(pci);

    cpu = new CPU(bus, iobus);
}

void System::Clock()
{
    cpu->Clock();
}

void System::Dump()
{
    cpu->Dump();
}