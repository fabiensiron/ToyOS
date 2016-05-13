/*
 * Copyright (c) 2016 Fabien Siron <fabien.siron@epita.fr>
 * All rights reserved.
 *
 * This program is Free Software, see COPYING for more info.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "libupkg/upkg_plist.h"
#include "libupkg/libupkg.h"

enum upkg_info_const {
	CONTENTS = 0,
	DESC,
	/* XXX not implemented yet */
	REQUIRING,
	REQUIRED_BY
};

char *upkg_info[] = {
	[CONTENTS] = "+CONTENTS",
	[DESC] = "+DESC",
	[REQUIRING] = "+REQUIRING",
	[REQUIRED_BY] = "+REQUIRED_BY"
};

static int
upkg_dump_description(struct upkg *pkg, const char *path) {
	char desc_path[80];
	FILE *f;
	int n;

	sprintf(desc_path, "%s/%s", path, upkg_info[DESC]);

	if((f = fopen(desc_path, "r")) == NULL) {
		return -1;
	}

	n = fread(pkg->description, 1, 80 * 25,f);

	pkg->description[n] = '\0';

	fclose(f);

	return 0;
}

static int
upkg_dump_packinglist(struct upkg *pkg, const char *path) {
	char desc_path[80];
	char packing_list[80 * 25];
	FILE *f;
	int n;

	sprintf(desc_path, "%s/%s", path, upkg_info[CONTENTS]);

	if((f = fopen(desc_path, "r")) == NULL) {
		return -1;
	}

	n = fread(packing_list, 1, 80 * 25,f);

	packing_list[n] = '\0';

	if (upkg_plist_extract(pkg, packing_list) == -1) {
		return -1;
	}

	fclose(f);

	return 0;
}

static int
upkg_dump_requiring(struct upkg *pkg __attribute__((unused)),
		    const char *path __attribute__((unused))) {
	/* XXX not yet implemented */

	return 0;
}

static int
upkg_dump_requiredby(struct upkg *pkg __attribute__((unused)),
		     const char *path __attribute__((unused))) {
	/* XXX not yet implemented */

	return 0;
}

static inline int
upkg_build_path(const char *name, char *path) {
	const char *dbdir, *rootdir;

	dbdir = upkg_config_get("UPKG_DBDIR");
	rootdir = upkg_config_get("UPKG_ROOT");

	sprintf(path, "%s%s/%s", rootdir, dbdir, name);

	return 0;
}

int
upkg_dump(struct upkg *pkg) {
	char path[80];

	/* extract package path */
	upkg_build_path(pkg->name, path);

	/* dump pkg info */
	if (upkg_dump_description(pkg, path) == -1) {
		return -1;
	}
	if (upkg_dump_packinglist(pkg, path) == -1) {
		return -1;
	}
	if (upkg_dump_requiring(pkg, path) == -1) {
		return -1;
	}
	if (upkg_dump_requiredby(pkg, path) == -1) {
		return -1;
	}

	return 0;
}

void
upkg_free(struct upkg *pkg) {
	upkg_plist_free(pkg);
}

static int
upkg_delete_special_file(struct upkg *pkg, char *name) {
	const char *dbdir, *rootdir;
	char path[80];

	dbdir = upkg_config_get("UPKG_DBDIR");
	rootdir = upkg_config_get("UPKG_ROOT");

	sprintf(path, "%s%s/%s/%s", rootdir, dbdir, pkg->name, name);

	return unlink(path);
}

static int
upkg_delete_special_files(struct upkg *pkg) {
	if (upkg_delete_special_file(pkg, upkg_info[DESC]) == -1) {
		return -1;
	}
	if (upkg_delete_special_file(pkg, upkg_info[CONTENTS]) == -1) {
		return -1;
	}
	return 0;
}

static int
upkg_delete_dir(struct upkg *pkg) {
	const char *dbdir, *rootdir;
	char path[80];

	dbdir = upkg_config_get("UPKG_DBDIR");
	rootdir = upkg_config_get("UPKG_ROOT");

	sprintf(path, "%s%s/%s", rootdir, dbdir, pkg->name);

	return rmdir(path);
}

int
upkg_delete(struct upkg *pkg) {
	if (upkg_plist_delete_files(pkg) == -1) {
		return -1;
	}
	if (upkg_delete_special_files(pkg) == -1) {
		return -1;
	}
	if (upkg_delete_dir(pkg) == -1) {
		return -1;
	}

	return 0;
}
