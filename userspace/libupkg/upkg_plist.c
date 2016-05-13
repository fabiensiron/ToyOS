/*
 * Copyright (c) 2016 Fabien Siron <fabien.siron@epita.fr>
 * All rights reserved.
 *
 * This program is Free Software, see COPYING for more info.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <libupkg/libupkg.h>
#include "upkg_plist.h"

#define FLAG_SYMBOL '@'
#define HOME_PATH_SYMBOL '~'
#define ABSOLUTE_PATH_SYMBOL '/'

static const char *upkg_plist_valid_flags[] = {
	FLAG_NAME,
	FLAG_AUTHOR,
	FLAG_INTERNAL,
	FLAG_VERSION,
	NULL,
};

static int
is_valid_flag(const char *flag) {
	int i, len;
	for (i = 0; upkg_plist_valid_flags[i] != NULL; ++i) {
		len = strlen(upkg_plist_valid_flags[i]);
		if (strncmp(upkg_plist_valid_flags[i], flag, len) == 0)
			return 1;
	}

	return 0;
}

static void
next_line(char **c) {
	for (;**c != '\0' && **c != '\n';++*c)
		;

	if (**c == '\n')
		++*c;
}

static int
get_line_len(char *c) {
	int i;

	for (i = 0; *c != '\0' && *c!= '\n'; ++i, ++c)
		;

	return i;
}

static inline int
is_a_flag(char *c) {
	return *c == FLAG_SYMBOL;
}

static inline int
is_a_path(char *c) {
	/* relative path are forbidden */
	return *c == HOME_PATH_SYMBOL || *c == ABSOLUTE_PATH_SYMBOL;
}

static inline int
dump_flag_arg(char *target, char *line, char *flag) {
	int flag_len, flag_arg_len;
	char *flag_arg;

	flag_len = strlen(flag);

	if (strlen(line + flag_len + 1) < 2) {
		return -1;
	}

	flag_arg = line + flag_len + 2;
	flag_arg_len = strlen(flag_arg);

	memcpy(target, flag_arg, flag_arg_len);

	target[flag_arg_len] = '\0';

	return 0;
}

int
upkg_plist_extract_flag(struct upkg *pkg, char *it) {
	/* XXX: flag name is a little bit silly because it's just the line */
	char flag[80];
	int line_len;

	line_len = get_line_len(it);
	memcpy(flag, it, line_len);
	flag[line_len] = '\0';

	if (!is_valid_flag(flag + 1)) {
		return -1;
	}

	if (strncmp(FLAG_NAME, flag+1,strlen(FLAG_NAME)) == 0) {
                # if 0
		memset(pkg->name, 0, 80);
		if (dump_flag_arg(pkg->name, flag, FLAG_NAME)
		    == -1) {
			return -1;
		}
		# endif
	} else if (strncmp(FLAG_AUTHOR, flag+1,strlen(FLAG_AUTHOR)) == 0) {
		if (dump_flag_arg(pkg->author, flag, FLAG_AUTHOR)
		    == -1) {
			return -1;
		}
	} else if (strncmp(FLAG_INTERNAL, flag+1,strlen(FLAG_INTERNAL))
		   == 0) {
		pkg->internal = 1;
	} else if (strncmp(FLAG_VERSION, flag+1,strlen(FLAG_VERSION))
		   == 0) {
		if (dump_flag_arg(pkg->version, flag, FLAG_VERSION)
		    == -1) {
			return -1;
		}
	}

	return 0;
}

static int
upkg_add_plist_elt(struct upkg *pkg, char *path) {
	struct upkg_plist_elt *elt;
	struct upkg_plist_elt *it;

	if ((elt = malloc(sizeof(struct upkg_plist_elt))) == NULL) {
		return -1;
	}
	strncpy(elt->path, path, 80);
	elt->next = NULL;
	it = &pkg->plist;
	while (it->next != NULL) {
		it = it->next;
	}
	it->next = elt;

	return 0;
}

void
upkg_plist_show(struct upkg *pkg) {
	struct upkg_plist_elt *it;

	it = &pkg->plist;
	while(it->next != NULL) {
		printf("%s\n", it->next->path);
		it = it->next;
	}
}

void
upkg_plist_free(struct upkg *pkg) {
	struct upkg_plist_elt *it, *tmp;

	it = &pkg->plist;
	it = it->next;
	while(it != NULL) {
		tmp = it;
		it = it->next;
		free (tmp);
	}
}

int
upkg_plist_extract_path(struct upkg *pkg, char *it) {
	char path[80];
	int len;
	len = get_line_len(it);
	memcpy(path, it, len);
	path[len] = '\0';

	return upkg_add_plist_elt(pkg, path);
}

int
upkg_plist_extract_line(struct upkg *pkg, char *it) {
	if (is_a_flag(it)) {
		return upkg_plist_extract_flag(pkg, it);
	} else if (is_a_path(it)){
		return upkg_plist_extract_path(pkg, it);
	} else {
		return -1;
	}
}

int
upkg_plist_extract(struct upkg *pkg, char *data) {
	char *it;

	/* for each line */
	for (it = data; *it != '\0'; next_line(&it)) {
		if (upkg_plist_extract_line(pkg, it) == -1) {
			return -1;
		}
	}

	return 0;
}

int
upkg_plist_delete_files(struct upkg *pkg) {
	struct upkg_plist_elt *elt;
	const char *rootdir;
	char path[80];
	int r = 0;

	rootdir = upkg_config_get("UPKG_ROOT");

	for (elt = pkg->plist.next; elt != NULL; elt = elt->next) {
	        sprintf(path, "%s%s", rootdir, elt->path);
		printf("unlink\n!");
		if (unlink(path) == -1) {
			r = -1;
		}
	}

	return r;
}
