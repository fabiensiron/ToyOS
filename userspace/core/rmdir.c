/*
 * This file is part of ToyOS and is released under the terms
 * of the NCSA / University of Illinois License - see LICENSE.md
 * Copyright (C) 2016 Fabien Siron <fabien.siron@epita.fr>
 *
 * rmdir
 *
 * Remove an empty directory
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static char *__progname;

int rm_path(char *path)
{
	char *p;

	while ((p = strrchr(path, '/')) != NULL) {
		/* Delete trailing slashes. */
		while (--p > path && *p == '/')
			;
		*++p = '\0';

		if (rmdir(path) < 0) {
			fprintf(stderr, "Error: %s, directory non empty", path);
			return (1);
		}
	}

	return (0);
}

void usage(void) {
	fprintf(stderr, "usage: %s [-p] directory ...\n", __progname);
	exit(1);
}

int
main(int argc, char *argv[]) {
	int ch, errors;
	int pflag;

	__progname = argv[0];

	pflag = 0;
	while ((ch = getopt(argc, argv, "p")) != -1) {
	   switch(ch) {
    		case 'p':
    			pflag = 1;
    			break;
    		default:
    			usage();
		}
    }
	argc -= optind;
	argv += optind;

	if (argc == 0)
		usage();

	for (errors = 0; *argv; argv++) {
		char *p;

		/* Delete trailing slashes, per POSIX. */
		p = *argv + strlen(*argv);
		while (--p > *argv && *p == '/')
			;
		*++p = '\0';

		if (rmdir(*argv) < 0) {
			fprintf(stderr, "Error: %s, directory non empty\n", *argv);
			errors = 1;
		} else if (pflag)
			errors |= rm_path(*argv);
	}

	return (errors);
}
