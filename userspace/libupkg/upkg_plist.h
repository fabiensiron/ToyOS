/*
 * Copyright (c) 2016 Fabien Siron <fabien.siron@epita.fr>
 * All rights reserved.
 *
 * This program is Free Software, see COPYING for more info.
 */

#ifndef _UPKG_PLIST_H_
# define _UPKG_PLIST_H_

# include "libupkg/libupkg.h"

# define FLAG_NAME "name"
# define FLAG_AUTHOR "author"
# define FLAG_INTERNAL "internal"
# define FLAG_VERSION "version"

int upkg_plist_extract_line(struct upkg *pkg, char *it);

int upkg_plist_extract(struct upkg *pkg, char *data);

void upkg_plist_show(struct upkg *pkg);
void upkg_plist_free(struct upkg *pkg);
int upkg_plist_delete_files(struct upkg *pkg);

#endif /* !_UPKG_PLIST_H_*/
