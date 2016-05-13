/*
 * Copyright (c) 2016 Fabien Siron <fabien.siron@epita.fr>
 * All rights reserved.
 *
 * This program is Free Software, see COPYING for more info.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "libupkg/libupkg.h"
#include "libupkg/upkg_plist.h"
#include "libupkg/upkg_config.h"
#include "libupkg/upkg_db.h"

/* TODO */
/* XXX Add some meta */
/* XXX Factorize code */
/* XXX Add one line description */

char *__progname;

void
print_plist(struct upkg *pkg) {
	struct upkg_plist_elt *node;
	char *path;

	foreach_in_plist(node, (&pkg->plist)) {
		path = node->path;
		printf("\t%s\n", path);
	}
}

int
info_each_packages(struct upkg_db *db, int Aflag, int Dflag, int Mflag) {
	struct upkg_repo_list *node;
	struct upkg *upkg;
	int r = 0;

	if (!Aflag)
		Aflag = 1;

	foreach_upkg(node, (&db->repos)) {
		upkg = &node->repo;

		if (upkg_dump(upkg) != -1) {
			if ((upkg->internal && Aflag == 2) ||
				!upkg->internal) {
				printf("%s\n", upkg->name);
				if (Dflag) {
					printf("    Description:\n");
					printf("\t%s\n",
					       upkg->description);
				}
				if (Mflag) {
					printf("    Author:\n");
					printf("\t%s\n", upkg->author);
					printf("    Version:\n");
					printf("\t%s\n", upkg->version);
				}
			}
		} else {
			r = -1;
		}
	}

	fflush(stdout);

	return r;
}

int
info_listed_packages(struct upkg_db *db, int argc, char *argv[],
		     int Dflag, int Lflag, int Mflag) {
	int i, r;
	struct upkg *upkg = malloc (sizeof(struct upkg));

	for (i = 0, r = 0; i < argc; ++i) {
		if(upkg_db_request(UPKG_DB_GET, db,
				   upkg, argv[i])) {
			printf("%s\n", argv[i]);
			if (upkg_dump(upkg) != -1) {
				if (Dflag) {
					printf("    Description:\n");
					printf("\t%s\n",
					       upkg->description);
				}
				if (Mflag) {
					printf("    Author:\n");
					printf("\t%s\n", upkg->author);
					printf("    Version:\n");
					printf("\t%s\n", upkg->version);
				}
				if (Lflag) {
					printf("    PList:\n");
					print_plist(upkg);
				}
			} else {
				printf("    Dump fail !\n");
				r = -1;
			}
		}
	}

	fflush(stdout);

	return r;
}

int
info_exist_packages(struct upkg_db *db, int argc, char *argv[]) {
	int i, r;

	for (i = 0, r = 0; i < argc; ++i) {
		if (upkg_db_request(UPKG_DB_EXIST, db, argv[i])) {
			printf("Found %s!\n", argv[i]);
		} else {
			printf("%s not found !\n", argv[i]);
			r = -1;
		}
	}

	fflush(stdout);

	return r;
}

void
usage(void) {
	fprintf(stderr, "usage: %s [-AacdELfm] [package(s)]\n", __progname);
	exit(1);
}

int
main(int argc, char *argv[]) {
	int c, r;
	int AFlag = 0;
	int DFlag = 0, EFlag = 0;
	int LFlag = 0, MFlag = 0;

	__progname = argv[0];

	while((c = getopt(argc, argv, "AacdELm")) != -1) {
		switch(c) {
		case 'A':
			AFlag = 2;
			break;
		case 'a':
			AFlag = 1;
			break;
		case 'c':
			DFlag = 1;
			break;
		case 'd':
			DFlag = 2;
			break;
		case 'E':
			EFlag = 1;
			break;
		case 'L':
			LFlag = 1;
			break;
		case 'm':
			MFlag = 1;
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	/* test zone */
	if ((argc == 0 && LFlag) || (argc == 0 && EFlag)) {
		fprintf(stderr, "Error: you cannot use this flag without packages in arguments...\n");
		usage();
	}

	if (argc && AFlag) {
		fprintf(stderr, "Error: you cannot use this flag with packages in arguments...\n");
		usage();
	}

	if (!upkg_check()) {
		fprintf(stderr, "Error: you don't have the right permissions.\n");
		exit(1);
	}

/*	if (upkg_db_lock()) {
		fprintf(stderr, "Error: database is already locked\n");
		exit(1);
		}*/

	struct upkg_db *upkg_db = upkg_db_get_empty();

	upkg_db_open(upkg_db);

	if (EFlag) {
		r = info_exist_packages(upkg_db, argc, argv);
	} else if (argc != 0) {
		r = info_listed_packages(upkg_db, argc, argv, DFlag,
					 LFlag, MFlag);
	} else {
		r = info_each_packages(upkg_db, AFlag, DFlag, MFlag);
	}

	upkg_db_release(upkg_db);

/*	upkg_db_unlock();*/

	return r;
}
