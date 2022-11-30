#include <emu/System.h>
#include <csignal>

extern System* _sys;

System::System()
{
	auto lam = [](int) {exit(1);};
	std::signal(SIGINT, lam);
	

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

	isa = new ISA(pci, iobus);
	agp = new AGP(pci);
	hostbridge = new HostBridge(pci, bus);
	nv2a = new NV2A(pci, bus);
	smbus = new SMBUS(pci, iobus);
	dram = new DramController(pci);
	usb1 = new OHCI(pci, bus, 0);
	usb2 = new OHCI(pci, bus, 1);

    cpu = new CPU(bus, iobus);
}

void System::Clock()
{
    cpu->Clock();
}

void System::Dump()
{
    cpu->Dump();
	bus->Dump_2bl();
}