#ifndef FILE_H
#define FILE_H

#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

#ifndef _POSIX_PATH_MAX
# define _POSIX_PATH_MAX 256
#endif

/* Structure to be stored in a tarball for each file */
typedef struct
{
	struct stat st;
	char path[_POSIX_PATH_MAX+1];
}
file_struct;

extern int errno;

#define BUF_SIZE 8192

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

int tar_flag;
int tar_cmd;

void error(const char *fmt, ...);
void call_error(const char *func, ...);
int file_copy(int fd_src, int fd_target, int size);
char * my_readlink(const char *lnk);
int find_file_in_list(const char *file, int argc, char **argv);
int file_exists_in_list(const char *file, int argc, char **argv);
char * remove_first_slashes(const char *str);
char * remove_last_slashes(const char *str);
int extract_next_file(int tfd, int argc, char **argv);
int extract_tarball(const char *tar_path, int argc, char **argv);
int add_file_to_tarball(const char *file, int tfd, const char *tar_path);
int create_tarball(const char *tar_path, int argc, char **argv);


#endif
