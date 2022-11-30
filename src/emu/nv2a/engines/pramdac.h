#pragma once

#include <cstdint>

class NV2A;

namespace NV2A_ENGINES
{

class PRAMDAC
{
private:
	uint32_t core_clock_coeff;
	uint64_t core_clock_freq;

	NV2A* nv2a;
public:
	PRAMDAC(NV2A* nv2a);

	void Write(uint32_t addr, uint32_t data);
};

}