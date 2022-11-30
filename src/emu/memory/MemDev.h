#pragma once

#include <cstdint>
#include <string>

class MemoryDevice
{
private:
    std::string _name;

    virtual uint8_t mem_ReadByte(uint32_t addr) = 0;
    virtual void mem_WriteByte(uint32_t addr, uint8_t data) = 0;
    virtual uint16_t mem_ReadWord(uint32_t addr) = 0;
    virtual void mem_WriteWord(uint32_t addr, uint16_t data) = 0;
    virtual uint32_t mem_ReadDoubleWord(uint32_t addr) = 0;
    virtual void mem_WriteDoubleWord(uint32_t addr, uint32_t data) = 0;
public:
    MemoryDevice(std::string name) : _name(name) {}
    std::string GetName() {return _name;}

    virtual uint32_t GetStart() = 0;
    virtual uint32_t GetEnd() = 0;

    template<typename T>
    T read(uint32_t addr)
    {
        if (std::is_same<T, uint8_t>::value || std::is_same<T, int8_t>::value)
        {
            return (T)mem_ReadByte(addr);
        }
        else if (std::is_same<T, uint16_t>::value || std::is_same<T, int16_t>::value)
        {
            return (T)mem_ReadWord(addr);
        }
        else if (std::is_same<T, uint32_t>::value || std::is_same<T, int32_t>::value)
        {
            return (T)mem_ReadDoubleWord(addr);
        }
        else
        {
            printf("Incorrect type of width uint%ld_t, read\n", sizeof(T)*8);
            exit(1);
        }
    }
    
    template<typename T>
    void write(uint32_t addr, T data)
    {
        if (std::is_same<T, uint8_t>::value)
        {
            return mem_WriteByte(addr, data);
        }
        if (std::is_same<T, uint16_t>::value)
        {
            return mem_WriteWord(addr, data);
        }
        if (std::is_same<T, uint32_t>::value)
        {
            return mem_WriteDoubleWord(addr, data);
        }
        else
        {
            printf("Incorrect type of width uint%d_t, write\n", sizeof(T)*8);
            exit(1);
        }
    }
};