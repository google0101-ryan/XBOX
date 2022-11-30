#pragma once

#include <cstdint>
#include <vector>
#include <fstream>

#include <emu/memory/MemDev.h>

class Bus
{
private:
    std::vector<MemoryDevice*> devices;
	uint8_t ram[0x03FFFFFF];
public:
    Bus();

    template<typename T>
    T read(uint32_t address)
    {
		if (address < 0x03FFFFFF)
			return *(T*)&ram[address];

        for (auto d : devices)
		{
            if (d->GetStart() <= address && d->GetEnd() >= address)
                return d->read<T>(address);
		}
				
		if ((address & 0xF0000000) == 0xF0000000)
		{
			printf("Unknown addr 0x%08x (read)\n", address);
			exit(1);
		}
		return 0;
    }
	
    template<typename T>
    void write(uint32_t address, T data)
    {
		if (address < 0x03FFFFFF)
		{
			*(T*)&ram[address] = data;
			return;
		}

        for (auto d : devices)
		{
            if (d->GetStart() <= address && d->GetEnd() >= address)
                return d->write<T>(address, data);
		}
		
		if ((address & 0xF0000000) == 0xF0000000)
		{
			printf("Unknown addr 0x%08x (write)\n", address);
			exit(1);
		}
    }

    void AttachDevice(MemoryDevice* dev);

	void Dump_2bl()
	{
		std::ofstream out("2bl.out");

		for (int i = 0x90000; i < 0x90000 + 1024*24; i++)
			out << ram[i];
		
		out.close();
	}
};