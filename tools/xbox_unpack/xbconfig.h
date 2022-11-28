#pragma once

extern	int	read_xbconfig_file(const char *cfg_fname);


#define	RC4_KEYLEN		16

#define	PATH_SEP				"/"

#define IMGFNAME_CABTMP			"cabtmp.cab"
#define	IMGFNAME_2BL			"2bl.img"
#define	IMGFNAME_XBOXKRNL		"xboxkrnl.img"
#define	IMGFNAME_REMAINDER		"remainder.img"


extern	unsigned int	CFG_2bl_size;
extern	unsigned int	CFG_2bl_address;

extern	unsigned int	CFG_2bl_kernelkey_offset;
extern	unsigned int	CFG_2bl_dwkernelsize_offset;
extern	unsigned int	CFG_2bl_dwkerneldatasize_offset;

extern	unsigned int	CFG_kernel_top_address;

extern	unsigned int	CFG_2bl_dwflashstart_hashsize;
extern	unsigned int	CFG_2bl_sha_hash_offset;

extern	unsigned int	CFG_kernel_dwdatasize_offset;
extern	unsigned int	CFG_kernel_dwdatarombase_offset;
extern	unsigned int	CFG_kernel_dwdatarambase_offset;

extern	unsigned int	CFG_kernel_address_adj;




extern	unsigned char CFG_RC4_key[RC4_KEYLEN];