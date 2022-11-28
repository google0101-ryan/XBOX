#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "xbconfig.h"

#include "cryptlib/crypt/rc4.h"
#include "xbunpack.h"

static int	load_rom_image(const char *romfname, unsigned char **p_rombuf, unsigned int *p_size);
static int	extract_2bl_image(unsigned char *p_rom_image, unsigned int size_rom_image, 
					unsigned char **p_2bl_image, unsigned int *p_size_2bl_image);
static int	decrypt_2bl_image(unsigned char *p_2bl_image, unsigned int size_2bl_image);
static int	extract_compressed_kernel_image(unsigned char *p_rom_image, unsigned int size_rom_image,
					unsigned char *p_2bl_image, unsigned int size_2bl_image,
					unsigned char **p_kernelZ_image, unsigned int *p_size_kernelZ_image);
static int	decrypt_compressed_kernel_image(unsigned char *p_kernelZ_image, unsigned int size_kernelZ_image,
						unsigned char *p_2bl_image, unsigned int size_2bl_image);

int do_xbunpack(const char* romfname, const char* outdirname)
{
	unsigned char	*p_rom_image;
	unsigned int	size_rom_image;
	unsigned char	*p_2bl_image;
	unsigned int	size_2bl_image;
	unsigned char	*p_kernelZ_image;
	unsigned int	size_kernelZ_image;
	char	outdir_prefix[BUFSIZ+1];
	char	fname_cabtmp[BUFSIZ+1];
	char	fname_2bl[BUFSIZ+1];
	char	fname_remainder[BUFSIZ+1];
	char	fname_kernel[BUFSIZ+1];
	FILE	*fh;

	strcpy(outdir_prefix, outdirname);
	strcat(outdir_prefix, PATH_SEP);
	
	strcpy(fname_cabtmp, outdir_prefix);
	strcat(fname_cabtmp, IMGFNAME_CABTMP);
	strcpy(fname_2bl, outdir_prefix);
	strcat(fname_2bl, IMGFNAME_2BL);
	strcpy(fname_kernel, outdir_prefix);
	strcat(fname_kernel, IMGFNAME_XBOXKRNL);
	strcpy(fname_remainder, outdir_prefix);
	strcat(fname_remainder, IMGFNAME_REMAINDER);
	
	printf("Loading ROM image file '%s'... ", romfname);
	if (!load_rom_image(romfname, &p_rom_image, &size_rom_image))
		return 0;
	
	printf("\nExtracting 2BL image...\n");
	if (!extract_2bl_image(p_rom_image, size_rom_image, &p_2bl_image, &size_2bl_image))
	{
		free(p_rom_image);
		return 0;
	}
	if (!decrypt_2bl_image(p_2bl_image, size_2bl_image))
	{
		free(p_rom_image);
		free(p_2bl_image);
		return 0;
	}

	printf("\nExtracting compressed KERNEL image... ");
	if (!extract_compressed_kernel_image(p_rom_image, size_rom_image,
					p_2bl_image, size_2bl_image,
					&p_kernelZ_image, &size_kernelZ_image)) {
		free(p_rom_image);
		free(p_2bl_image);
		return 0;
	}

	printf("\nDecrypting compressed KERNEL image... ");
	if (!decrypt_compressed_kernel_image(p_kernelZ_image, size_kernelZ_image,
	 				p_2bl_image, size_2bl_image)) {
	 	free(p_rom_image);
	 	free(p_2bl_image);
	 	free(p_kernelZ_image);
	 	return 0;
	}

	printf("Decompressing KERNEL image (compressed size: %u)... ", size_kernelZ_image);
	if (!create_kernel_cab_from_compressed(fname_cabtmp, IMGFNAME_CABTMP, p_kernelZ_image, size_kernelZ_image))
	{
		free(p_rom_image);
		free(p_2bl_image);
		free(p_kernelZ_image);
		return 0;
	}


	printf("\nWriting 2BL image file '%s'...\n", fname_2bl);
	if (!(fh = fopen(fname_2bl, "wb"))) {
		fprintf(stderr, "Cannot write output 2bl file!\n");
		free(p_rom_image);
		free(p_2bl_image);
		return 0;
	}
	if (fwrite(p_2bl_image, 1, size_2bl_image, fh) != size_2bl_image) {
		fprintf(stderr, "Error writing output 2bl file!\n");
		fclose(fh);
		unlink(fname_2bl);
		free(p_rom_image);
		free(p_2bl_image);
		return 0;
	}
	fclose(fh);

	// We no longer need 2bl image, so free it up
	free(p_2bl_image);

	printf("\nWriting Kernel image file '%s'...\n", fname_kernel);
	if (!(fh = fopen(fname_kernel, "wb"))) {
		fprintf(stderr, "Cannot write output kernel file!\n");
		free(p_rom_image);
		free(p_kernelZ_image);
		return 0;
	}
	if (fwrite(p_kernelZ_image, 1, size_kernelZ_image, fh) != size_kernelZ_image) {
		fprintf(stderr, "Error writing output kernel file!\n");
		fclose(fh);
		free(p_rom_image);
		free(p_kernelZ_image);
		return 0;
	}
	fclose(fh);

	return 1;
}

static int decrypt_2bl_image(unsigned char* p_2bl_image, unsigned int size_2bl_image)
{
	RC4_KEY rc4_key;

	RC4_set_key(&rc4_key, RC4_KEYLEN, CFG_RC4_key);
	RC4(&rc4_key, size_2bl_image, p_2bl_image, p_2bl_image);

	return 1;
}

static int extract_2bl_image(unsigned char *p_rom_image, unsigned int size_rom_image, 
					unsigned char **p_2bl_image, unsigned int *p_size_2bl_image)
{
	unsigned char* p_buf;
	unsigned int rom_image_offset;

	rom_image_offset = (CFG_2bl_address & (size_rom_image - 1));

	if ((rom_image_offset + CFG_2bl_size) > size_rom_image)
	{
		fprintf(stderr, "CFG 2bl address/size problem!\n");
		return 0;
	}
	if (!(p_buf = malloc(CFG_2bl_size)))
	{
		fprintf(stderr, "Not enough memory to load 2bl!\n");
		return 0;
	}

	memcpy(p_buf, (p_rom_image + rom_image_offset), CFG_2bl_size);

	memset((p_rom_image + rom_image_offset), 0, CFG_2bl_size);

	*p_2bl_image = p_buf;
	*p_size_2bl_image = CFG_2bl_size;

	return 1;
}

static int extract_compressed_kernel_image(unsigned char *p_rom_image, unsigned int size_rom_image,
					unsigned char *p_2bl_image, unsigned int size_2bl_image,
					unsigned char **p_kernelZ_image, unsigned int *p_size_kernelZ_image)
{
	unsigned char	*p_kernelZ;
	unsigned int	kernelZ_size;
	unsigned int	kernel_data_size;
	unsigned int	kernel_top_rom_offset;
	unsigned int	kernel_rom_offset;

	if ( ((CFG_2bl_kernelkey_offset + RC4_KEYLEN) > size_2bl_image) ||
		((CFG_2bl_dwkernelsize_offset + sizeof(int)) > size_2bl_image) ||
		((CFG_2bl_dwkerneldatasize_offset + sizeof(int)) > size_2bl_image) ) {
		fprintf(stderr, "Invalid config params for kernel extract!\n");
		return 0;
	}

	kernelZ_size = *(unsigned int*)(p_2bl_image + CFG_2bl_dwkernelsize_offset);
	kernel_data_size = *(unsigned int*)(p_2bl_image + CFG_2bl_dwkerneldatasize_offset);

	kernel_top_rom_offset = (CFG_kernel_top_address & (size_rom_image - 1));

	if ((kernelZ_size + kernel_data_size - 1) > kernel_top_rom_offset)
	{
		fprintf(stderr, "Invalid config params for kernel extract!\n");
		return 0;
	}

	kernel_rom_offset = (kernel_top_rom_offset + 1) - kernel_data_size - kernelZ_size;

	printf("Kernel is at 0x%08x in ROM\n", kernel_rom_offset);

	if (!(p_kernelZ = malloc(kernelZ_size)))
	{
		fprintf(stderr, "Not enough memory for kernel extract!\n");
		return 0;
	}

	memcpy(p_kernelZ, (p_rom_image + kernel_rom_offset), kernelZ_size);

	// Now go back to ROM image and wipe-out the area used by compressed kernel
	memset((p_rom_image + kernel_rom_offset), 0, kernelZ_size);

	// And the area used by the kernel initialized data section
	memset((p_rom_image + kernel_rom_offset + kernelZ_size), 0, kernel_data_size);

	*p_kernelZ_image = p_kernelZ;
	*p_size_kernelZ_image = kernelZ_size;

	return 1;
}

static int load_rom_image(const char *romfname, unsigned char **p_rombuf, unsigned int *p_size)
{
	FILE* fh;
	unsigned char* rom_buf;
	unsigned int filesize;

	if (!(fh = fopen(romfname, "rb")))
	{
		fprintf(stderr, "Cannot open input ROM file: '%s'!\n", romfname);
		return 0;
	}

	fseek(fh, 0L, SEEK_END);
	filesize = ftell(fh);

	if (!filesize || (filesize & (filesize-1))) {
		fprintf(stderr, "Invalid filesize for ROM file: %u\n", filesize);
		fclose(fh);
		return 0;
	}

	if (!(rom_buf = malloc(filesize))) {
		fprintf(stderr, "Not enough memory to load ROM!\n");
		fclose(fh);
		return 0;
	}

	fseek(fh, 0L, SEEK_SET);
	if ((fread(rom_buf, 1, filesize, fh) != filesize)) {
		fprintf(stderr, "Error reading ROM file!\n");
		free(rom_buf);
		fclose(fh);
		return 0;
	}

	fclose(fh);

	*p_rombuf = rom_buf;
	*p_size = filesize;
	return 1;
}

static int decrypt_compressed_kernel_image(unsigned char *p_kernelZ_image, unsigned int size_kernelZ_image,
						unsigned char *p_2bl_image, unsigned int size_2bl_image)
{
	RC4_KEY	rc4_key;
	unsigned char	kernel_key[RC4_KEYLEN];

	if (((CFG_2bl_kernelkey_offset + RC4_KEYLEN) > size_2bl_image)) {
		fprintf(stderr, "Invalid config params for kernel decrypt!\n");
		return 0;
	}
	memcpy(kernel_key, (p_2bl_image + CFG_2bl_kernelkey_offset), RC4_KEYLEN);

	RC4_set_key(&rc4_key, RC4_KEYLEN, kernel_key);
	RC4(&rc4_key, size_kernelZ_image, p_kernelZ_image, p_kernelZ_image);

	return 1;
}

static void write_byte(FILE* fh, int val)
{
	fputc((val & 0xff), fh);
}

static void
write_word(FILE *fh, unsigned int val)
{
	write_byte(fh, (val & 0xff));
	write_byte(fh, ((val >> 8) & 0xff));
}

static void
write_dword(FILE *fh, unsigned int val)
{
	write_word(fh, (val & 0xffff));
	write_word(fh, ((val >> 16) & 0xffff));
}

static void
write_string(FILE *fh, const char *str)
{
	do {
		write_byte(fh, *str);
	} while(*str++);
}



static void write_CFHEADER(FILE* fh, unsigned int cabsize, unsigned int cffile_off)
{
	write_byte(fh, 'M');
	write_byte(fh, 'S');
	write_byte(fh, 'C');
	write_byte(fh, 'F');

	write_dword(fh, 0);	// reserved

	write_dword(fh, cabsize);	// size of this cab file in bytes

	write_dword(fh, 0);	// reserved

	write_dword(fh, cffile_off);	// offset of first CFFILE entry

	write_dword(fh, 0);	// reserved

	write_byte(fh, 0x03);	// cab format version
	write_byte(fh, 0x01);

	write_word(fh, 1);	// # of CFFOLDERs in this cab file

	write_word(fh, 1);	// # of CFFILEs in this cab file

	write_word(fh, 0);	// flags

	write_word(fh, 0x3039);	// set ID

	write_word(fh, 0);	// number of this cab file in a set
}

static void write_CFFOLDER(FILE* fh, unsigned int num_cfdata, unsigned int cfdata_off)
{
	write_dword(fh, cfdata_off);

	write_word(fh, num_cfdata);

	write_word(fh, 0x1103);
}

static void write_CFFILE(FILE *fh, const char *fname, unsigned int uncmp_file_size)
{
	write_dword(fh, uncmp_file_size);

	write_dword(fh, 0);

	write_word(fh, 0);

	write_word(fh, 0x27dc);

	write_word(fh, 0x6bdd);

	write_word(fh, 0x0020);

	write_string(fh, fname);
}

static unsigned int write_CFDATA(FILE *fh, int this_cmp, int this_uncmp, char *cfdata_buf)
{
	unsigned char *p;
	unsigned int	csum;
	int	i;
	csum = 0;
	p = (unsigned char *)cfdata_buf;
	for (i = 0; i < this_cmp; i++) {
		csum += *p++;
	}

	csum = 0;
	write_dword(fh, csum);	// checksum
	write_word(fh, this_cmp);
	write_word(fh, this_uncmp);

	fwrite(cfdata_buf, 1, this_cmp, fh);

	return (4 + 2 + 2 + this_cmp);
}

int create_kernel_cab_from_compressed(const char *cab_temp_fname, const char *cab_internal_fname, unsigned char *p_kernelZ_image, unsigned int size_kernelZ_image)
{
	FILE* fh_cab;
	long total_cmp, total_uncmp;
	unsigned int stream_offset;
	int this_cmp, this_uncmp;
	long	cab_total_size;
	int	cab_num_cfdata;
	long	cab_uncmp_file_size;
	long	cab_cfdata_off, cab_cffile_off;
	unsigned short	*p_short;

	if (!(fh_cab = fopen(cab_temp_fname, "wb"))) {
		fprintf(stderr, "Cannot create temporary .cab file!\n");
		return 0;
	}

	// write place holder header items
	write_CFHEADER(fh_cab, 0, 0);	// cabsize, cffile_off
	write_CFFOLDER(fh_cab, 0, 0);
	cab_cffile_off = ftell(fh_cab);
	write_CFFILE(fh_cab, cab_internal_fname, 0);
	cab_cfdata_off = ftell(fh_cab);

	cab_total_size = ftell(fh_cab);
	cab_num_cfdata = 0;
	cab_uncmp_file_size = 0;

	total_cmp = 0;
	total_uncmp = 0;
	stream_offset = 0;
	while (1)
	{
		if (stream_offset >= size_kernelZ_image) {
			break;
		}
		p_short = (unsigned short *)(p_kernelZ_image + stream_offset);
		this_cmp = *p_short++;
		this_uncmp = *p_short++;

		if (this_cmp == 0) {
			break;
		}

		cab_total_size += write_CFDATA(fh_cab, this_cmp, this_uncmp, (void *)p_short);
		cab_num_cfdata += 1;
		cab_uncmp_file_size += this_uncmp;

		total_cmp += this_cmp;
		total_uncmp += this_uncmp;
		stream_offset += (this_cmp + 4);
	}

	fseek(fh_cab, 0L, SEEK_SET);
	write_CFHEADER(fh_cab, cab_total_size, cab_cffile_off);
	write_CFFOLDER(fh_cab, cab_num_cfdata, cab_cfdata_off);
	write_CFFILE(fh_cab, cab_internal_fname, cab_uncmp_file_size);

	fclose(fh_cab);
	return 1;
}