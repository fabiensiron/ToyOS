/*
 * Copyright (c) 2016 Fabien Siron <fabien.siron@epita.fr>
 * All rights reserved.
 *
 * This program is Free Software, see COPYING for more info.
 */

#ifndef _UPKG_DB_H_
# define _UPKG_DB_H_

#include "libupkg/libupkg.h"

struct upkg_db {
	struct upkg_repo_list repos;
};

typedef enum {
	UPKG_DB_PRINTALL,
	UPKG_DB_EXIST,
	UPKG_DB_GET,
	UPKG_DB_DELETE
} upkg_db_request_t;

int upkg_check(void);
int upkg_db_open(struct upkg_db *db);
int upkg_db_lock(void);
int upkg_db_unlock(void);
int upkg_db_request(upkg_db_request_t request, struct upkg_db *,...);
void upkg_db_release(struct upkg_db *db);
struct upkg_db *upkg_db_get_empty(void);
int upkg_db_synchronize(struct upkg_db *db);

#endif /*_UPKG_DB_H_*/
