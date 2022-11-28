#include "rc4.h"
#include "rc4_locl.h"

char* RC4_version = "RC4 part of SSLeay 0.8.2 04-Nov-1997";

char* RC4_options()
{
	if (sizeof(RC4_INT) == 1)
		return ("rc4(idx,char)");
	else
		return ("rc4(idx,int)");
}

void RC4_set_key(RC4_KEY* key, int len, unsigned char* data)
{
	RC4_INT tmp;
	int id1, id2;
	RC4_INT *d;
	unsigned int i;

	d = &(key->data[0]);
	for (i = 0; i < 256; i++)
		d[i]=i;
	key->x=0;
	key->y=0;
	id1=id2=0;

#define SK_LOOP(n) { \
		tmp=d[(n)]; \
		id2 = (data[id1] + tmp + id2) & 0xff; \
		if (++id1 == len) id1=0; \
		d[(n)]=d[id2]; \
		d[id2]=tmp; }

	for (i = 0; i < 256; i += 4)
	{
		SK_LOOP(i+0);
		SK_LOOP(i+1);
		SK_LOOP(i+2);
		SK_LOOP(i+3);
	}
}