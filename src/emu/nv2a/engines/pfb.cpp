#include <emu/nv2a/engines/pfb.h>

using namespace NV2A_ENGINES;

void PFB::Write(uint32_t addr, uint32_t data)
{
	addr &= 0xFFFF;

	regs[addr] = data;
}