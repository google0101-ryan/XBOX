#pragma once

#include <cstdint>
#include <vector>

#include <emu/memory/MemDev.h>

class Bus
{
private:
    std::vector<MemoryDevice*> devices;
public:
    Bus();

    template<typename T>
    T read(uint32_t address)
    {
        for (auto d : devices)
            if (d->GetStart() <= address && d->GetEnd() >= address)
                return d->read<T>(address);
        printf("Unknown addr 0x%08x (read)\n", address);
        exit(1);
    }

    void AttachDevice(MemoryDevice* dev);
};