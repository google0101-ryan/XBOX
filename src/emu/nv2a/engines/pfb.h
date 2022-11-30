#pragma once

#include <cstdint>

namespace NV2A_ENGINES
{

class PFB
{
private:
	uint32_t regs[0x1000];
public:
	void Write(uint32_t addr, uint32_t data);
};

}