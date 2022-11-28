#pragma once

#include <emu/io/IoBus.h>
#include <map>

class PCIDevice
{
private:
	uint8_t config_space[256];

public:
	void write_config_byte(uint8_t offset, uint8_t data)
	{
		config_space[offset] = data;
		if (offset == 0x10)
			RemapBAR(0);
		if (offset == 0x14)
			RemapBAR(1);
		if (offset == 0x18)
			RemapBAR(2);
		if (offset == 0x1C)
			RemapBAR(3);
		if (offset == 0x20)
			RemapBAR(4);
		if (offset == 0x24)
			RemapBAR(5);
	}
	
	void write_config_word(uint8_t offset, uint16_t data)
	{
		*(uint16_t*)&config_space[offset] = data;
		if (offset == 0x10)
			RemapBAR(0);
		if (offset == 0x14)
			RemapBAR(1);
		if (offset == 0x18)
			RemapBAR(2);
		if (offset == 0x1C)
			RemapBAR(3);
		if (offset == 0x20)
			RemapBAR(4);
		if (offset == 0x24)
			RemapBAR(5);
	}
	
	void write_config_dword(uint8_t offset, uint32_t data)
	{
		*(uint32_t*)&config_space[offset] = data;
		if (offset == 0x10)
			RemapBAR(0);
		if (offset == 0x14)
			RemapBAR(1);
		if (offset == 0x18)
			RemapBAR(2);
		if (offset == 0x1C)
			RemapBAR(3);
		if (offset == 0x20)
			RemapBAR(4);
		if (offset == 0x24)
			RemapBAR(5);
	}

	uint32_t read_config_dword(uint8_t offset)
	{
		return *(uint32_t*)&config_space[offset];
	}
public:
	PCIDevice(uint16_t vendor_id, uint16_t device_id, uint8_t class_code, uint8_t sub_class_code);

	virtual void RemapBAR(int bar) {}
};

class PCIBus : public IoDev
{
private:
	std::map<uint32_t, PCIDevice*> devices;

	uint32_t cur_dev;
	uint32_t cur_reg;

	void WriteDoubleWord(uint16_t port, uint32_t data) override;
public:
	PCIBus(IoBus* iobus);

	void RegisterPCIDevice(int bus, int dev, int func, PCIDevice* device);
};