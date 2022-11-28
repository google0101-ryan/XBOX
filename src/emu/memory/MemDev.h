#pragma once

#include <cstdint>
#include <string>

class MemoryDevice
{
private:
    std::string _name;

    virtual uint8_t ReadByte(uint32_t addr) = 0;
    virtual void WriteByte(uint32_t addr, uint8_t data) = 0;
    virtual uint16_t ReadWord(uint32_t addr) = 0;
    virtual void WriteWord(uint32_t addr, uint16_t data) = 0;
    virtual uint32_t ReadDoubleWord(uint32_t addr) = 0;
    virtual void WriteDoubleWord(uint32_t addr, uint32_t data) = 0;
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
            return (T)ReadByte(addr);
        }
        else if (std::is_same<T, uint16_t>::value || std::is_same<T, int16_t>::value)
        {
            return (T)ReadWord(addr);
        }
        else if (std::is_same<T, uint32_t>::value || std::is_same<T, int32_t>::value)
        {
            return (T)ReadDoubleWord(addr);
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
            return WriteByte(addr, data);
        }
        else
        {
            printf("Incorrect type of width uint%d_t, write\n", sizeof(T)*8);
            exit(1);
        }
    }
};