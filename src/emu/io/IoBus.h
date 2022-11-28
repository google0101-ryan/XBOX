#pragma once

#include <emu/io/iodev.h>
#include <array>

class IoBus
{
private:
	std::array<IoDev*, 0xFFFF> io_devices;
public:
	void RegisterDevice(IoDev* dev, uint16_t port)
	{
		io_devices[port] = dev;
		printf("Registered device %s on 0x%04x\n", dev->GetName().c_str(), port);
	}

	void UnregisterDevice(uint16_t port)
	{
		io_devices[port] = nullptr;
	}

	template<typename T>
	void write(uint16_t addr, T data)
	{
		if (!io_devices[addr])
		{
			printf("Unknown I/O device for port 0x%04x\n", addr);
			exit(1);
		}

		io_devices[addr]->write<T>(addr, data);
	}

	template<typename T>
	T read(uint16_t addr)
	{
		if (!io_devices[addr])
		{
			printf("Unknown I/O device for port 0x%04x\n", addr);
			exit(1);
		}

		return io_devices[addr]->read<T>(addr);
	}
};