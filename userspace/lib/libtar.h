/* This file is part of libtar, a fork of mytar
 *
 * This code is licensed under the MIT license
 * Copyright (C) 2016 Jason Lu
 *
 */

#ifndef FILE_H
#define FILE_H

#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

#ifndef _POSIX_PATH_MAX
# define _POSIX_PATH_MAX 256
#endif

/* Commands */
#define TAR_CREATE  1
#define TAR_EXTRACT 2
#define TAR_LIST    3
#define TAR_UPDATE  4
#define TAR_APPEND  5

/* Flags */
#define TAR_VERBOSE 1
#define TAR_FILE    1<<1
#define TAR_FOLLOW_SYMLINKS 1<<2

/* Structure to be stored in a tarball for each file */
typedef struct
{
    struct stat st;
    char path[_POSIX_PATH_MAX+1];
}
file_struct;

int verbose;
int followSymlink;

int create(const char *path, char ** files, int number);
int extract(const char *path);
int list(const char *path);
int update(const char *path, char ** files, int number);
int append(const char *path, char ** files, int number);
#endif
