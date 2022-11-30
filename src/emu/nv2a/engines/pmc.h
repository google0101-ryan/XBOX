#pragma once

#include <cstdint>

class NV2A;

namespace NV2A_ENGINES
{

class PMC
{
private:
	uint32_t pending_interrupts;
	uint32_t enabled_interrupts;

	NV2A* nv2a;
public:
	PMC(NV2A* nv2a);

	uint32_t Read(uint32_t off);
};

}