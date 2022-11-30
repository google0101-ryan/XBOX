#include <emu/nv2a/engines/pbus.h>
#include <emu/nv2a/nv2a.h>

using namespace NV2A_ENGINES;

PBUS::PBUS(NV2A* nv2a)
: nv2a(nv2a)
{}

void PBUS::Write(uint32_t addr, uint32_t data)
{
	switch (addr)
	{
	default:
		printf("Write to unknown PBUS addr 0x%08x\n", addr);
		break;
	}
}