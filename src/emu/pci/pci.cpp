#include <emu/pci/pci.h>

PCIDevice::PCIDevice(uint16_t vendor_id, uint16_t device_id, uint8_t class_code, uint8_t sub_class_code)
{
	write_config_word(0x00, device_id);
	write_config_word(0x02, vendor_id);

	write_config_byte(0x08, class_code);
	write_config_byte(0x09, sub_class_code);
}

PCIBus::PCIBus(IoBus* iobus)
: IoDev("Peripheral Component Interconnect Bus")
{
	iobus->RegisterDevice(this, 0x0cf8);
	iobus->RegisterDevice(this, 0x0cfc);
}

void PCIBus::RegisterPCIDevice(int bus, int dev, int func, PCIDevice* device)
{
	uint32_t addr = (bus << 16) | (dev << 11) | (func << 8);
	devices[addr] = device;
}

void PCIBus::WriteDoubleWord(uint16_t port, uint32_t data)
{
	if (port == 0xcf8)
	{
		cur_dev = data;
		cur_reg = data & 0xFF;
		int bus = (cur_dev >> 16) & 0xFF;
		int dev = (cur_dev >> 11) & 0x1F;
		int func = (cur_dev >> 8) & 0x07;
		printf("Selected PCI device %d.%d:%d.%x (0x%08x)\n", bus, dev, func, cur_reg, data);
	}
	else if (port == 0xcfc)
	{
		if (!devices[cur_dev & 0xFFFF00])
		{
			int bus = (cur_dev >> 16) & 0xFF;
			int dev = (cur_dev >> 11) & 0x1F;
			int func = (cur_dev >> 8) & 0x07;
			printf("Write to unknown PCI device %d.%d:%d.%x\n", bus, dev, func, cur_reg);
			exit(1);
		}

		devices[cur_dev & 0xFFFF00]->write_config_dword(cur_reg, data);
	}
	else
	{
		IoDev::WriteDoubleWord(port, data);
	}
}

uint32_t PCIBus::ReadDoubleWord(uint16_t port)
{
	switch (port)
	{
	case 0xCFC:
	case 0xCFD:
	case 0xCFE:
	case 0xCFF:
		return devices[cur_dev & 0xFFFF00]->read_config_dword(port - 0xCFC);
	default:
		return IoDev::ReadDoubleWord(port);
	}
}