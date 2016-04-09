/*
 * This file is part of ToyOS and is released under the terms
 * of the NCSA / University of Illinois License - see LICENSE.md
 * Copyright (C) 2016 Fabien Siron <fabien.siron@epita.fr>
 *
 * pwd
 *
 * Print the working directory
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

char *__progname;

static void
usage(void)
{
	fprintf(stderr, "usage: %s [-LP]\n", __progname);
	exit(1);
}

int
main(int argc, char *argv[])
{
	char p[1024], a[1024], *r;
	int c, z, lFlag = 0;
	struct stat file;

	__progname = argv[0];

	/* we do that just to have a compliant posix interface */
	while((c = getopt(argc, argv, "LP")) != -1) {
		switch(c) {
		case 'L':
			lFlag = 0;
			break;
		case 'P':
			lFlag = 1;
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 0)
		usage();

	r = getcwd(p, 1024);
	lstat(p, &file);


	if (r == NULL) {
		exit(1);
	}

	if (lFlag && S_ISLNK(file.st_mode)) {
		z = readlink(p, a, sizeof(a) - 1);
		if (z != -1) {
			a[z] = '\0';
		}
		puts(a);
	} else {
		puts(p);
	}

	fflush(stdout);

	return 0;
}
