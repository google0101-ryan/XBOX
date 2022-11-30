#pragma once

#include <cstdint>

class NV2A;

namespace NV2A_ENGINES
{

class PBUS
{
private:
	NV2A* nv2a;
public:
	PBUS(NV2A* nv2a);

	void Write(uint32_t addr, uint32_t data);
};

}