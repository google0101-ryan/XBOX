#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define RC4_INT unsigned int

typedef struct rc4_key_st
{
	RC4_INT x, y;
	RC4_INT data[256];
} RC4_KEY;

#ifndef NOPROTO

char* RC4_options(void);
void RC4_set_key(RC4_KEY* key, int len, unsigned char* data);
void RC4(RC4_KEY* key, unsigned long len, unsigned char* indata, unsigned char* outdata);

#else

char *RC4_options();
void RC4_set_key();
void RC4();

#endif

#ifdef __cplusplus
}
#endif