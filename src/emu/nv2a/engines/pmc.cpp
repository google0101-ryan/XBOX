#include <emu/nv2a/engines/pmc.h>
#include <emu/nv2a/nv2a.h>

using namespace NV2A_ENGINES;

PMC::PMC(NV2A* nv2a)
: nv2a(nv2a)
{
	pending_interrupts = enabled_interrupts = 0;
}

uint32_t PMC::Read(uint32_t addr)
{
	switch (addr)
	{
	case 0:
		return 0x02A000A3;
	default:
		printf("Read from unknown PMC register 0x%08x\n", addr);
		return 0;
	}
}