#include <emu/nv2a/engines/pramdac.h>
#include <emu/nv2a/nv2a.h>

using namespace NV2A_ENGINES;

PRAMDAC::PRAMDAC(NV2A* nv2a)
: nv2a(nv2a)
{}

#define NV_PRAMDAC_NVPLL_COEFF_MDIV                        0x000000FF
#define NV_PRAMDAC_NVPLL_COEFF_NDIV                        0x0000FF00
#define NV_PRAMDAC_NVPLL_COEFF_PDIV                        0x00070000

static constexpr int NV2A_FREQ = 16666666;

void PRAMDAC::Write(uint32_t addr, uint32_t data)
{
	switch (addr)
	{
	case 0x00680500:
	{
		core_clock_coeff = data;

		uint32_t m = data & NV_PRAMDAC_NVPLL_COEFF_MDIV;
		uint32_t n = (data & NV_PRAMDAC_NVPLL_COEFF_NDIV) >> 8;
		uint32_t p = (data & NV_PRAMDAC_NVPLL_COEFF_PDIV) >> 16;

		if (!m)
			core_clock_freq = 0;
		else
			core_clock_freq = (NV2A_FREQ * n) / (1 << p) / m;
	}
	break;
	case 0x0068050C:
		break;
	default:
		printf("Write to unknown PRAMDAC register 0x%08x\n", addr);
		exit(1);
	}
}