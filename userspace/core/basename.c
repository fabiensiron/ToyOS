/*
 * This file is part of ToyOS and is released under the terms
 * of the NCSA / University of Illinois License - see LICENSE.md
 * Copyright (C) 2016 Fabien Siron <fabien.siron@epita.fr>
 *
 * basename
 *
 * parse basename components
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *__progname;

#define BNAME_LEN 1024

char *
basename(char *path, char *bname)
{
	char *endp, *startp;
	size_t len;

	if (path == NULL || *path == '\0') {
		bname[0] = '.';
		bname[1] = '\0';
		return bname;
	}

	endp = path + strlen(path) -1;
	while (endp > path && *endp == '/')
		endp--;

	if (endp == path && *endp == '/') {
		bname[0] = '/';
		bname[1] = '\0';
		return bname;
	}

	startp = endp;
	while(startp > path && *(startp - 1) != '/')
		startp--;

	len = endp - startp + 1;
	if (len >= BNAME_LEN) {
		return NULL;
	}

	memcpy(bname, startp, len);
	bname[len] = '\0';

	return bname;
}


static void
usage(void)
{
	fprintf(stderr, "usage: %s [-p] directory ...\n", __progname);
	exit(1);
}

int
main(int argc, char *argv[])
{
	char *p, bname[BNAME_LEN];
	int c;

	__progname = argv[0];

	while((c = getopt(argc, argv, "")) != -1) {
		switch(c) {
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 1 && argc != 2)
		usage();

	if (**argv == '\0') {
		puts("");
		exit(0);
	}

	p = basename(*argv, bname);

	if (p == NULL)
		exit(1);

	if (*++argv) {
		size_t suffixlen, stringlen, off;

		suffixlen = strlen(*argv);
		stringlen = strlen(p);

		if (suffixlen < stringlen) {
			off = stringlen - suffixlen;
			if (!strcmp(p + off, *argv))
				p[off] = '\0';
		}
	}

	puts(p);

	fflush(stdout);

	return 0;
}
