#include <emu/memory/Rom.h>
#include <filesystem>
#include <fstream>

Rom::Rom(std::string mcpx_path)
: MemoryDevice("Read-Only Flash Memory")
{
    if (!std::filesystem::exists(mcpx_path))
    {
        printf("ERR: Couldn't find the file specified: %s\n", mcpx_path.c_str());
        exit(1);
    }

    std::ifstream file(mcpx_path, std::ios::binary | std::ios::ate);

    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size != 512)
    {
        printf("ERR: File %s too %s (should be 512 bytes, is %ld)\n", mcpx_path.c_str(), size > 512 ? "big" : "small", size);
        exit(1);
    }

    file.read((char*)mcpx, size);
    file.close();

    file.open("xbox.rom", std::ios::binary | std::ios::ate);
    size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size != 1024*1024)
    {
        printf("ERR: File xbox.rom too %s (should be 1MB, is %ld)\n", size > 1024*1024 ? "big" : "small", size);
        exit(1);
    }

    file.read((char*)rom, size);
    file.close();

    printf("Sucessfully loaded MCPX ROM\n");
}

uint8_t Rom::ReadByte(uint32_t address)
{
    if ((address & 0xFFFFFE00) == 0xFFFFFE00 && isMcpxEnabled)
    {
        return mcpx[address & 0b111111111];
    }
    else
    {
        return rom[address & 0xFFFFF];
    }
    printf("[%s]ERR: Could not read from address 0x%08x\n", GetName().c_str(), address);
    exit(1);
}

uint16_t Rom::ReadWord(uint32_t address)
{
    if ((address & 0xFFFFFE00) == 0xFFFFFE00 && isMcpxEnabled)
    {
        return *(uint16_t*)&mcpx[address & 0b111111111];
    }
    else
    {
        return *(uint16_t*)&rom[address & 0xFFFFF];
    }
    printf("[%s]ERR: Could not read from address 0x%08x\n", GetName().c_str(), address);
    exit(1);
}

uint32_t Rom::ReadDoubleWord(uint32_t address)
{
    if ((address & 0xFFFFFE00) == 0xFFFFFE00)
    {
        return *(uint32_t*)&mcpx[address & 0b111111111];
    }
    else
    {
        return *(uint32_t*)&rom[address & 0xFFFFF];
    }
    printf("[%s]ERR: Could not read from address 0x%08x\n", GetName().c_str(), address);
    exit(1);
}

void Rom::WriteByte(uint32_t address, uint8_t)
{
    printf("[%s]ERR: Could not write to address 0x%08x\n", GetName().c_str(), address);
    exit(1);
}

void Rom::WriteWord(uint32_t address, uint16_t)
{
    printf("[%s]ERR: Could not write to address 0x%08x\n", GetName().c_str(), address);
    exit(1);
}

void Rom::WriteDoubleWord(uint32_t address, uint32_t)
{
    printf("[%s]ERR: Could not write to address 0x%08x\n", GetName().c_str(), address);
    exit(1);
}