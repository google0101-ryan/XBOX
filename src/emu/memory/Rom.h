#pragma once

#include <emu/memory/MemDev.h>

class Rom : public MemoryDevice
{
private:
    bool isMcpxEnabled = true;
    uint8_t mcpx[512];
    uint8_t rom[1024*1024];

    uint8_t mem_ReadByte(uint32_t address) override;
    void mem_WriteByte(uint32_t address, uint8_t data) override;
    uint16_t mem_ReadWord(uint32_t address) override;
    void mem_WriteWord(uint32_t address, uint16_t data) override;
    uint32_t mem_ReadDoubleWord(uint32_t address) override;
    void mem_WriteDoubleWord(uint32_t address, uint32_t data) override;
public:
    Rom(std::string mcpx);

    uint32_t GetStart() override {return 0xFF000000;}
    uint32_t GetEnd() override {return 0xFFFFFFFF;}

    void DisableRom();
};