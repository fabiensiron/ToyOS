/*
 * Copyright (c) 2016 Fabien Siron <fabien.siron@epita.fr>
 * All rights reserved.
 *
 * This program is Free Software, see COPYING for more info.
 */
#define _POSIX_C_SOURCE 200809L
#define __TOYOS__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/file.h>
#include <sys/types.h>

#include <libupkg/upkg_config.h>
#include <libupkg/upkg_db.h>
#include <libupkg/libupkg.h>

#define CLRSTR(X) (X[0] = '\0')

static int fd_pkgdir = -1;

//static struct upkg_db upkg_db = {};
/* XXX: Do the list functions really need to be in this module? */

int
upkg_check(void) {
	/* XXX: move to upkg.c */
	return 1;
}

static void
upkg_db_add_entry_at(struct upkg_repo_list *node, struct upkg *repo) {
	struct upkg_repo_list *new_node =
		malloc(sizeof(struct upkg_repo_list));
	struct upkg_repo_list *next_node = node->next;

	memcpy(&new_node->repo,repo,sizeof(struct upkg));

	node->next = new_node;
	new_node->next = next_node;
}

static int
upkg_db_remove_entry_at(struct upkg_repo_list *node) {
	struct upkg_repo_list *next_node = node->next;

	if(!next_node)
		return -1;

	node->next = node->next->next;
//	upkg_free(&next_node->repo);
	free(next_node);

	return 0;
}

static void
upkg_db_remove_entries(struct upkg_repo_list *node) {
	while(node->next != NULL)
		upkg_db_remove_entry_at(node);
}

static int
upkg_db_add_entry(struct upkg_db *db, struct upkg *repo) {
	struct upkg_repo_list *repos_list = &db->repos;

	/* to avoid a later sort, we try to keep an alphabetical order */
	/* we let the first node empty as it is a sentinelle */
	while(repos_list->next != NULL &&
	      strcmp(repos_list->next->repo.name, repo->name) < 0)
		repos_list = repos_list->next;

	upkg_db_add_entry_at(repos_list, repo);

	return 1;
}

struct upkg_db *
upkg_db_get_empty(void) {
	struct upkg_db *db = malloc(sizeof(struct upkg_db));

	memset(&db->repos, 0, sizeof(struct upkg_repo_list));

	return db;
}

int
upkg_db_open(struct upkg_db *db) {
	struct dirent *dirent;

#ifdef __TOYOS__

	// if (fd_pkgdir == -1) {
	const char *dbdir, *rootdir;
	char path[80];

	dbdir = upkg_config_get("UPKG_DBDIR");
	rootdir = upkg_config_get("UPKG_ROOT");

	sprintf(path, "%s%s", rootdir, dbdir);
    // fd_pkgdir = open(rootdir, O_RDWR);
	// }

	/* short ls */
	DIR *dirp = opendir(path);

#else

	if (fd_pkgdir == -1) {
		const char *dbdir, *rootdir;
		char path[80];

		dbdir = upkg_config_get("UPKG_DBDIR");
		rootdir = upkg_config_get("UPKG_ROOT");

		sprintf(path, "%s%s", rootdir, dbdir);
		fd_pkgdir = open(rootdir, O_RDWR);
	}

	/* short ls */
	DIR *dirp = fdopendir(fd_pkgdir);

        rewinddir(dirp);
#endif

	if (!dirp) {
		return -1;
	}

	struct upkg *current_repo = malloc(sizeof(struct upkg));

	dirent = readdir(dirp);
	while (dirent != NULL) {
		/* XXX: put these names dynamic */
		current_repo->internal = 0;
		CLRSTR(current_repo->name);
		CLRSTR(current_repo->path);
		CLRSTR(current_repo->author);
		CLRSTR(current_repo->version);
		CLRSTR(current_repo->description);

		if (dirent->d_name[0] != '.') {
			size_t d_name_len = strlen(dirent->d_name);

			memcpy(&current_repo->name, dirent->d_name,
			       d_name_len);

			current_repo->name[d_name_len] = '\0';

			upkg_db_add_entry(db, current_repo);
		}

		dirent = readdir(dirp);
	}
	free(dirp);
	free(current_repo);

	return 0;
}

void
upkg_db_release(struct upkg_db *db) {
	upkg_db_remove_entries(&db->repos);
	free(db);
}

int
upkg_db_lock(void)
{
	const char *dbdir, *rootdir;
	char path[80];

	dbdir = upkg_config_get("UPKG_DBDIR");
	rootdir = upkg_config_get("UPKG_ROOT");

	sprintf(path, "%s%s", rootdir, dbdir);

	fd_pkgdir = open(path, O_RDONLY);

	if (flock(fd_pkgdir, LOCK_EX | LOCK_NB) == 0) {
		return 0;
	}

	fprintf(stderr, "Package database is already locked... Wait...");

	while(flock(fd_pkgdir, LOCK_EX | LOCK_NB)) ;

	fprintf(stderr, "Done\n");

	return 0;
}

int
upkg_db_unlock(void)
{
	int r = flock(fd_pkgdir, LOCK_UN);

	close(fd_pkgdir);

	return r;
}

/* for debug purpose only */
static void
upkg_db_printall(struct upkg_db *db) {
	struct upkg_repo_list *repos = &db->repos;

	for(;repos->next != NULL; repos = repos->next)
		printf("Found: %s\n", repos->next->repo.name);
}

static int
upkg_db_exist(struct upkg_db *db, const char *name) {
	struct upkg_repo_list *repos = &db->repos;

	for(;repos->next != NULL; repos = repos->next)
		if (strcmp(repos->next->repo.name, name) == 0)
			return 1;

	return 0;
}

static int
upkg_db_get(struct upkg_db *db, struct upkg *upkg, const char *name) {
	struct upkg_repo_list *repos = &db->repos;

	for(;repos->next != NULL; repos = repos->next) {
		if (strcmp(repos->next->repo.name, name) == 0) {
			memcpy(upkg, &repos->next->repo, sizeof(struct upkg));
			return 1;
		}
	}

	return 0;
}

static int
upkg_db_delete(struct upkg_db *db, const char *name) {
	struct upkg_repo_list *repos = &db->repos;

	for(;repos->next != NULL; repos = repos->next) {
		if (strcmp(repos->next->repo.name, name) == 0) {
			if (upkg_db_remove_entry_at(repos) == -1)
				return -1;
			else
				return 1;
		}
	}

	return 0;
}

int
upkg_db_request(upkg_db_request_t request, struct upkg_db *db,...)
{
	int r = 1;
	va_list argp;
	va_start(argp, db);

	switch(request)
	{
	case UPKG_DB_PRINTALL:
	{
		upkg_db_printall(db);
		break;
	}
	case UPKG_DB_GET:
	{
		struct upkg *pkg = va_arg(argp, void *);
		const char *name = va_arg(argp, char *);
		r = upkg_db_get(db, pkg, name);
		break;
	}
	case UPKG_DB_EXIST:
	{
		r = upkg_db_exist(db, va_arg(argp, char *));
		break;
	}
	case UPKG_DB_DELETE:
	{
		const char *name = va_arg(argp, char *);
		r = upkg_db_delete(db, name);
		break;
	}}

	va_end(argp);

	return r;
}

/* 1 for yes, 0 for no */
static inline int
ask_yes_no (char *question, ...) {
	/* XXX rewrite function without gotos */
	/* XXX move to utils */
	char input[3];
	char *newline;

try_again:
	do {
		va_list argp;
		va_start(argp, question);
		vfprintf(stderr, question, argp);
		va_end(argp);

	} while(!fgets(input, 3, stdin));

	newline = strchr(input, '\n');
	if (!newline) {
		while (!newline && fgets(input, sizeof input, stdin))
			newline = strchr(input, '\n');
		goto try_again;
	}
	if (input[0] != 'y' && input[0] != 'n') {
		goto try_again;
	}

	return input[0] == 'y';
}

int
upkg_db_synchronize(struct upkg_db *db) {
	struct upkg_db *disk_db = upkg_db_get_empty();
	struct upkg_repo_list *disk_repos;
	struct upkg_repo_list *repos;

	if(upkg_db_open(disk_db) == -1) {
		return -1;
	}

	disk_repos = &disk_db->repos;
	repos = &db->repos;

	while(repos->next != NULL || disk_repos->next != NULL) {
		if (repos->next == NULL) {
			/* deletions disk_repos->next->repo */
			if(ask_yes_no("Do you really want to delete %s (y/n)? ",
				      disk_repos->next->repo.name)) {
				if (upkg_dump(&disk_repos->next->repo)
				    == -1) {
					return -1;
				}
				if (upkg_delete(&disk_repos->next->repo)
				    == -1) {
					printf("FAIL\n");
					return -1;
				}
			}

			if (disk_repos->next) {
				disk_repos = disk_repos->next;
			}
			continue;
		}
		if (disk_repos->next == NULL) {
			/* saving repos->next->repo */
			if (repos->next) {
				repos = repos->next;
			}
			continue;
		}
		int ord = strcmp(repos->next->repo.name,
				 disk_repos->next->repo.name);
		if (ord == 0) {
			repos = repos->next;
			disk_repos = disk_repos->next;
		} else if (ord < 0) {
			/* saving repos->next->repo */
			repos = repos->next;
		} else if (ord > 0) {
			/* deletions disk_repos->next->repo */
			if(ask_yes_no("Do you really want to delete %s (y/n)? ",
				      disk_repos->next->repo.name)) {
				if (upkg_dump(&disk_repos->next->repo)
				    == -1) {
					return -1;
				}
				if (upkg_delete(&disk_repos->next->repo)
				    == -1) {
					return -1;
				}
			}

			disk_repos = disk_repos->next;
		}
	}

	upkg_db_release(disk_db);

	return 0;
}
