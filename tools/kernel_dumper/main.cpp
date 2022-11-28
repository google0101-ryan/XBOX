#include <stdint.h>
#include <string.h>
#include <fstream>

uint8_t ram[0x04000000];

struct DOSHeader
{
	char sig[2];
	uint8_t reserved1[0x3A];
	uint32_t pe_header_start;
};

struct PeHeader
{
	uint32_t magic;
	uint16_t machine;
	uint16_t number_of_sections;
	uint32_t time_date_stamp;
	uint32_t pointer_to_symbol_table;
	uint32_t number_of_symbols;
	uint16_t size_of_optional_header;
	uint16_t characteristics;
};

struct Pe32OptionalHeader 
{
	uint16_t mMagic; // 0x010b - PE32, 0x020b - PE32+ (64 bit)
	uint8_t  mMajorLinkerVersion;
	uint8_t  mMinorLinkerVersion;
	uint32_t mSizeOfCode;
	uint32_t mSizeOfInitializedData;
	uint32_t mSizeOfUninitializedData;
	uint32_t mAddressOfEntryPoint;
	uint32_t mBaseOfCode;
	uint32_t mBaseOfData;
	uint32_t mImageBase;
	uint32_t mSectionAlignment;
	uint32_t mFileAlignment;
	uint16_t mMajorOperatingSystemVersion;
	uint16_t mMinorOperatingSystemVersion;
	uint16_t mMajorImageVersion;
	uint16_t mMinorImageVersion;
	uint16_t mMajorSubsystemVersion;
	uint16_t mMinorSubsystemVersion;
	uint32_t mWin32VersionValue;
	uint32_t mSizeOfImage;
	uint32_t mSizeOfHeaders;
	uint32_t mCheckSum;
	uint16_t mSubsystem;
	uint16_t mDllCharacteristics;
	uint32_t mSizeOfStackReserve;
	uint32_t mSizeOfStackCommit;
	uint32_t mSizeOfHeapReserve;
	uint32_t mSizeOfHeapCommit;
	uint32_t mLoaderFlags;
	uint32_t mNumberOfRvaAndSizes;
};

int main()
{
	std::ifstream krnl("xboxkrnl.exe", std::ios::ate);
	size_t size = krnl.tellg();
	krnl.seekg(0, std::ios::beg);
	uint8_t* data = new uint8_t[size];
	krnl.read((char*)data, size);
	krnl.close();

	DOSHeader* dos_header = (DOSHeader*)data;

	printf("PE header starts at 0x%08x\n", dos_header->pe_header_start);

	PeHeader* pe_header = (PeHeader*)(data + dos_header->pe_header_start);

	Pe32OptionalHeader* opt_header = (Pe32OptionalHeader*)(data + dos_header->pe_header_start + sizeof(PeHeader));

	printf("Entry is 0x%08x\n", opt_header->mAddressOfEntryPoint + 0x80010000);

	uint8_t* kernel_buf = (ram + 0x10000);

	memcpy(kernel_buf, data, size);

	std::ifstream krnl_d("xbox.rom", std::ios::ate);
	size = krnl_d.tellg();
	krnl_d.seekg(0, std::ios::beg);
	uint8_t* kernel_data = new uint8_t[size];
	krnl_d.read((char*)kernel_data, size);
	krnl_d.close();

	uint32_t kernel_ram_addr = *(uint32_t*)(kernel_buf + 0x34);
	uint32_t kernel_data_size = *(uint32_t*)(kernel_buf + 0x2C);

	printf("Kernel data segment gets copied to 0x%08x (%i bytes)\n", kernel_ram_addr, kernel_data_size);

	kernel_ram_addr &= 0xFFFFF;

	uint8_t* kernel_data_buf = (kernel_data + 0xF9400);

	memcpy(&ram[kernel_ram_addr], kernel_data_buf, kernel_data_size);

	std::ofstream r("kernel.memdump");

	for (int i = 0; i < 0x10000 + size; i++)
		r << ram[i];
	
	r.close();

	return 0;
}