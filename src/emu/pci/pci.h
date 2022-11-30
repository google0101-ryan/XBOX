#pragma once

#include <emu/io/IoBus.h>
#include <map>

class PCIDevice
{
protected:
	uint8_t config_space[256];

public:
	void write_config_byte(uint8_t offset, uint8_t data)
	{
		config_space[offset] = data;
		if (offset >= 0x10 && offset < 0x14)
			RemapBAR(0);
	}
	
	void write_config_word(uint8_t offset, uint16_t data)
	{
		*(uint16_t*)&config_space[offset] = data;
		if (offset >= 0x10 && offset < 0x14)
			RemapBAR(0);
	}
	
	// Virtual because we may need to override BAR behaviour
	virtual void write_config_dword(uint8_t offset, uint32_t data)
	{
		printf("Writing 0x%02x to config space off 0x%02x\n", data, offset);
		*(uint32_t*)&config_space[offset] = data;
		if (offset >= 0x10 && offset < 0x14)
			RemapBAR(0);
	}

	uint32_t read_config_dword(uint8_t offset)
	{
		return *(uint32_t*)&config_space[offset];
	}
public:
	PCIDevice(uint16_t vendor_id, uint16_t device_id, uint8_t class_code, uint8_t sub_class_code);

	virtual void RemapBAR(int) {}
};

class PCIBus : public IoDev
{
private:
	std::map<uint32_t, PCIDevice*> devices;

	uint32_t cur_dev;
	uint32_t cur_reg;

	void WriteDoubleWord(uint16_t port, uint32_t data) override;
	uint32_t ReadDoubleWord(uint16_t port) override;
public:
	PCIBus(IoBus* iobus);

	void RegisterPCIDevice(int bus, int dev, int func, PCIDevice* device);
};