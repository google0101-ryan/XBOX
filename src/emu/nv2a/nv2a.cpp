#include <emu/nv2a/nv2a.h>

NV2A::NV2A(PCIBus* pci, Bus* bus)
: PCIDevice(0x10de, 0x02a0, 0x03, 0x00),
MemoryDevice("NV2A XGPU")
{
	pci->RegisterPCIDevice(1, 0, 0, this);
	bus->AttachDevice(this);

	// Engine initialization
	pmc = new NV2A_ENGINES::PMC(this);
	pbus = new NV2A_ENGINES::PBUS(this);
	pfb = new NV2A_ENGINES::PFB();
	pramdac = new NV2A_ENGINES::PRAMDAC(this);
}

void NV2A::RemapBAR(int bar)
{
	uint8_t config_space_off = bar * 4 + 0x10;

	uint32_t addr = read_config_dword(config_space_off);

	bool is_io = addr & 1;
	if (is_io)
		addr &= 0xFFFFFFFC;
	else
		addr &= 0xFFFFFFF0;

	printf("Remapping NV2A bus %s to 0x%08x\n", is_io ? "I/O" : "memory", addr);

	if (!is_io)
		start_address = addr;
}

uint8_t NV2A::mem_ReadByte(uint32_t addr)
{
	printf("Read8 from unknown NV2A address 0x%08x\n", addr);
	exit(1);
}

void NV2A::mem_WriteByte(uint32_t addr, uint8_t data)
{
	printf("Write8 to unknown NV2A address 0x%08x\n", addr);
	exit(1);
}

uint16_t NV2A::mem_ReadWord(uint32_t addr)
{
	printf("Read16 from unknown NV2A address 0x%08x\n", addr);
	exit(1);
}

void NV2A::mem_WriteWord(uint32_t addr, uint16_t data)
{
	printf("Write16 to unknown NV2A address 0x%08x\n", addr);
	exit(1);
}

uint32_t NV2A::mem_ReadDoubleWord(uint32_t addr)
{
	addr -= start_address;

	if (addr < 0x1000)
		return pmc->Read(addr);
	else if (addr >= 0x101000 && addr < 0x102000)
		return 0;

	printf("Read from unknown NV2A address 0x%08x\n", addr);
	exit(1);
}

void NV2A::mem_WriteDoubleWord(uint32_t addr, uint32_t data)
{
	addr -= start_address;

	if (addr >= 0x1000 && addr < 0x2000)
		return pbus->Write(addr, data);
	else if (addr >= 0x100000 && addr < 0x101000)
		return pfb->Write(addr, data);
	else if (addr >= 0x680000 && addr < 0x681000)
		return pramdac->Write(addr, data);

	printf("Write to unknown NV2A address 0x%08x\n", addr);
	exit(1);
}