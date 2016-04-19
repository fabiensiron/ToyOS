#include "lib/libstar.h"
#include <stdio.h>
#include <stdlib.h>

void help(const char *argv0, const char *msg)
{
	if(msg) {
		fprintf(stderr, "ERROR: %s\n\n", msg);
	}
	fprintf(stderr,
			"USAGE: %s [OPTIONS] [FILE...]\n\n"
			"Where OPTIONS is a string containing one of the following:\n\n"
			"c     Create the archive\n"
			"x     Extract the archive\n"
			"t     Test the archive\n"
			"u     Update the archive\n\n"
			"Or additional flags:\n\n"
			"f     Input/Output from/to the specified file\n"
			"v     Verbose output\n"
	,argv0);

	exit(-1);
}

int parse_options(const char *opts, const char *argv0)
{
	tar_flag = 0;
	tar_cmd = 0;

	while(*opts) {
		switch (*opts) {
			case 'x':
				if(tar_cmd) help(argv0, NULL);
				tar_cmd = TAR_EXTRACT;
				break;

			case 'c':
				if(tar_cmd) help(argv0, NULL);
				tar_cmd = TAR_CREATE;
				break;

			case 't':
				if(tar_cmd) help(argv0, NULL);
				tar_cmd = TAR_LIST;
				break;

			case 'u':
				if(tar_cmd) help(argv0, NULL);
				tar_cmd = TAR_UPDATE;
				break;

			case 'r':
				if(tar_cmd) help(argv0, NULL);
				tar_cmd = TAR_APPEND;
				break;

			case 'v':
				tar_flag |= TAR_VERBOSE;
				break;

			case 'f':
				tar_flag |= TAR_FILE;
				break;

			case 'h':
				tar_flag |= TAR_FOLLOW_SYMLINKS;
				break;

			default :
				help(argv0, "Unknown option");
		}
		opts++;
	}

	if(!tar_cmd) help(argv0, NULL);
}

int main(int argc, char **argv)
{
	int i;
	int files_num = 0;
	char ** files = NULL;
	char *tarball_name = NULL;

	if(argc < 2) {
		help(argv[0], NULL);
	}

	parse_options(argv[1], argv[0]);
	argc -= 2;
	argv += 2;

	if(tar_flag & TAR_FILE) {
		tarball_name = argv[0];
		argc --;
		argv ++;
	}

	switch (tar_cmd)
	{
		case TAR_CREATE:
		case TAR_UPDATE:
		case TAR_APPEND:
			create_tarball(tarball_name, argc, argv);
			break;

		case TAR_EXTRACT:
		case TAR_LIST:
			extract_tarball(tarball_name, argc, argv);
			break;
	}
}
