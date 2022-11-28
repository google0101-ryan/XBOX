#pragma once

#include <emu/memory/MemDev.h>

class Rom : public MemoryDevice
{
private:
    bool isMcpxEnabled = true;
    uint8_t mcpx[512];
    uint8_t rom[1024*1024];

    uint8_t ReadByte(uint32_t address) override;
    void WriteByte(uint32_t address, uint8_t data) override;
    uint16_t ReadWord(uint32_t address) override;
    void WriteWord(uint32_t address, uint16_t data) override;
    uint32_t ReadDoubleWord(uint32_t address) override;
    void WriteDoubleWord(uint32_t address, uint32_t data) override;
public:
    Rom(std::string mcpx);

    uint32_t GetStart() override {return 0xFF000000;}
    uint32_t GetEnd() override {return 0xFFFFFFFF;}

    void DisableRom();
};