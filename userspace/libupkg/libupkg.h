/*
 * Copyright (c) 2016 Fabien Siron <fabien.siron@epita.fr>
 * All rights reserved.
 *
 * This program is Free Software, see COPYING for more info.
 */

#ifndef _LIBUPKG_H_
# define _LIBUPKG_H_

# include "libupkg/upkg_config.h"

typedef unsigned char bool;

struct upkg_plist_elt {
	char path[80];
	struct upkg_plist_elt *next;
};

struct upkg {
	bool internal;
	char name[80];
	char path[80];
	char author[80];
	char version[80];
	char description[80 * 25];
	struct upkg_plist_elt plist;
};

struct upkg_repo_list {
	struct upkg repo;
	struct upkg_repo_list *next;
};

# define foreach_upkg(repo, repos)					\
	for (repo = repos->next;					\
	     repo != NULL;						\
	     repo = repo->next)

int upkg_dump(struct upkg *);
void upkg_free(struct upkg *);

# define foreach_in_plist(p, plist)		\
	for (p = plist->next;			\
	     p != NULL;				\
	     p = p->next)

int upkg_delete(struct upkg *);


#endif /*_LIBUPKG_H_*/
