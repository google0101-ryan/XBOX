#pragma once

#include <cstdint>
#include <string>

class IoDev
{
private:
	std::string name;

protected:
	virtual uint8_t ReadByte(uint16_t)
	{
		printf("Read of unknown length 'byte' to device %s\n", name.c_str());
		return 0;
	}
    virtual void WriteByte(uint16_t, uint8_t)
	{
		printf("Write of unknown length 'byte' to device %s\n", name.c_str());
	}
    virtual uint16_t ReadWord(uint16_t)
	{
		printf("Read of unknown length 'word' to device %s\n", name.c_str());
		return 0;
	}
    virtual void WriteWord(uint16_t, uint16_t)
	{
		printf("Write of unknown length 'word' to device %s\n", name.c_str());
	}
    virtual uint32_t ReadDoubleWord(uint16_t)
	{
		printf("Read of unknown length 'double word' to device %s\n", name.c_str());
		return 0;
	}
    virtual void WriteDoubleWord(uint16_t, uint32_t)
	{
		printf("Write of unknown length 'double word' to device %s\n", name.c_str());
	}
public:
	IoDev(std::string name) : name(name) {}

	std::string GetName() {return name;}

	template<typename T>
    T read(uint16_t addr)
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
    void write(uint16_t addr, T data)
    {
        if (std::is_same<T, uint8_t>::value)
        {
            return WriteByte(addr, data);
        }
		else if (std::is_same<T, uint32_t>::value || std::is_same<T, int32_t>::value)
        {
            return WriteDoubleWord(addr, data);
        }
        else
        {
            printf("Incorrect type of width uint%ld_t, write\n", sizeof(T)*8);
            exit(1);
        }
    }
};