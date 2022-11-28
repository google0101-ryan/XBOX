#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "xbconfig.h"
#include "xbunpack.h"

char* Pname;

void
usage(void)
{
	fprintf(stderr,
		"Usage:\t%s -unpack config.ini filename.rom outdirname\n", Pname);
	fprintf(stderr, "\t\t\t-or-\n");
	fprintf(stderr,
		"\t%s -pack config.ini indirname filename.rom\n", Pname);
}

int main(int argc, char* argv[])
{
	int want_unpack = 0;

	Pname = argv[0];

	if (argc != 5)
	{
		usage();
		exit(1);
	}

	if (!strcmp(argv[1], "-unpack")) {
		want_unpack = 1;
	} else if (!strcmp(argv[1], "-pack")) {
		want_unpack = 0;
	} else {
		usage();
		exit(1);
	}

	if (!read_xbconfig_file(argv[2])) {
		exit(1);
	}
	if (want_unpack) {
		if (!do_xbunpack(argv[3], argv[4])) {
			exit(1);
		}
	}

	return 0;
}