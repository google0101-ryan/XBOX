#include <emu/memory/Bus.h>

Bus::Bus()
{}

void Bus::AttachDevice(MemoryDevice* dev)
{
    devices.push_back(dev);
    printf("Registered device %s from 0x%08x to 0x%08x\n", dev->GetName().c_str(), dev->GetStart(), dev->GetEnd());
}