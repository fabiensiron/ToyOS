#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <dirent.h>
#include "libtar.h"

extern int errno;

#define BUF_SIZE 8192

void error(const char *fmt, ...)
{
	va_list l;
	va_start(l, fmt);
	vfprintf(stderr, fmt, l);
	va_end(l);
	exit(-1);
}

void call_error(const char *func, ...)
{
	error("%s: %s\n", func, strerror(errno));
}

int file_copy(int fd_src, int fd_target, int size)
{
	int n, r_size;
	char buf[BUF_SIZE];

	while(size > 0)
	{
		r_size = (BUF_SIZE < size ? BUF_SIZE : size);

		n = read(fd_src, buf, r_size);
		if(n == -1) {
			call_error("read");
		}
		if(n == 0) break;

		if(write(fd_target, buf, n) != n) {
			call_error("write");
		}
		size -= n;
	}
}

char * my_readlink(const char *lnk)
{
	static char path[_POSIX_PATH_MAX+1];
	int n;
	if((n = readlink(lnk, path, _POSIX_PATH_MAX)) == -1) {
		call_error(lnk);
	}
	path[n] = '\0';
	return path;
}

int find_file_in_list(const char *file, int argc, char **argv)
{
	int i = -1;
	while(argc-- > 0) {
		i++;
		if(strcmp(*(argv++), file) == 0) {
			return i;
		}
	}
	return i;
}

int file_exists_in_list(const char *file, int argc, char **argv)
{
	if(argc <= 0) return 1;
	return find_file_in_list(file, argc, argv);
}

char * remove_first_slashes(const char *str)
{
	static char path[_POSIX_PATH_MAX+1];
	int i=0;

	while(str[i] == '/') {
		i++;
	}

	strcpy(path, str+i);
	return path;
}

char * remove_last_slashes(const char *str)
{
	static char path[_POSIX_PATH_MAX+1];
	int i = strlen(str)-1;

	strcpy(path, str);
	while(i>0 && path[i] == '/') {
		path[i] = '\0';
		i--;
	}
	return path;
}

int extract_next_file(int tfd, int argc, char **argv)
{
	file_struct fs;
	int read_val;
	char * file_buf;
	int fd;

	if((read_val = read(tfd, &fs, sizeof(file_struct))) == -1) {
		call_error("read");
	}

	if(read_val == 0)
		return 0;

	if(!file_exists_in_list(fs.path, argc, argv))
		return 1;

	if(tar_cmd == TAR_LIST || (tar_flag & TAR_VERBOSE)) {
		printf("%s\n", fs.path);
	}

	if(S_ISREG(fs.st.st_mode)) {
		if(tar_cmd == TAR_LIST) {
			if(lseek(tfd, fs.st.st_size, SEEK_CUR) == -1) {
				call_error("lseek");
			}
		}
		else {
			if((fd = open(fs.path, O_WRONLY| O_CREAT, fs.st.st_mode)) == -1) {
				call_error(fs.path);
			}
			file_copy(tfd, fd, fs.st.st_size);

			close(fd);

			if(chown(fs.path, fs.st.st_uid, fs.st.st_gid) == -1) {
				call_error(fs.path);
			}
		}
	}
	else if(S_ISLNK(fs.st.st_mode)) {
		if(tar_cmd == TAR_LIST) {
			if(lseek(tfd, fs.st.st_size, SEEK_CUR) == -1) {
				call_error("lseek");
			}
		}
		else {
			file_buf = (char *)malloc(sizeof(char) * (fs.st.st_size + 1));
			if(!file_buf) {
				call_error("malloc");
			}
			if(read(tfd, file_buf, fs.st.st_size) == -1) {
				call_error("read");
			}
			if(symlink(file_buf, fs.path) == -1) {
				call_error("symlink");
			}
			// if(lchown(fs.path, fs.st.st_uid, fs.st.st_gid) == -1) {
			//	call_error(fs.path);
			// }
			free(file_buf);
		}
	}
	else if(S_ISDIR(fs.st.st_mode)) {
		if(tar_cmd != TAR_LIST) {
			if(mkdir(fs.path, fs.st.st_mode) == -1) {
				call_error(fs.path);
			}
		}
	}
	else if(S_ISCHR(fs.st.st_mode)) {
	}
	else if(S_ISBLK(fs.st.st_mode)) {
	}
	else if(S_ISFIFO(fs.st.st_mode)) {
	}
	else if(S_ISSOCK(fs.st.st_mode)) {
	}

	return 1;
}

int extract_tarball(const char *tar_path, int argc, char **argv)
{
	int fd;
	if(tar_path) {
		if((fd = open(tar_path, O_RDONLY)) == -1) {
			call_error(tar_path);
		}
	}
	else fd = STDIN_FILENO; //Else read from STDIN

	while(extract_next_file(fd, argc, argv));

	if(fd > 2) close(fd);
}

int add_file_to_tarball(const char *file, int tfd, const char *tar_path)
{
	int fd;
	file_struct fs;
	file_struct fs_old;
	char * file_buf;
	DIR * dir;
	struct dirent * d_ent;
	int write_file = 1;

	if(lstat(file, &fs.st) == -1) {
		call_error(file);
	}

	if(tar_cmd == TAR_UPDATE) {
		if(tar_path) {
			if((fd = open(tar_path, O_RDONLY)) == -1) {
				call_error(tar_path);
			}
		}
		else fd = STDIN_FILENO;

		while(read(fd, &fs_old, sizeof(file_struct)) > 0) {
			if(strcmp(file, fs_old.path) == 0) {
				if(fs_old.st.st_mtime == fs.st.st_mtime && fs_old.st.st_ctime == fs.st.st_ctime) {
					write_file = 0; /* Don't schedule to update this file, since it hasn't changed */
					break;
				}
			}
			if(!S_ISDIR(fs_old.st.st_mode)) {
				if(lseek(fd, fs_old.st.st_size, SEEK_CUR) == -1) {
					call_error("lseek");
				}
			}
		}

		if(fd > 2) close(fd);
	}

	if((tar_flag & TAR_VERBOSE) && write_file) {
		printf("%s\n", file);
	}

	if(S_ISLNK(fs.st.st_mode) && (tar_flag & TAR_FOLLOW_SYMLINKS)) {
		if(lstat(my_readlink(file), &fs.st) == -1) {
			call_error(file);
		}
	}

	strncpy(fs.path, remove_first_slashes(file), _POSIX_PATH_MAX);
	if(write_file) {
		if(write(tfd, &fs, sizeof(file_struct)) != sizeof(file_struct)) {
			call_error("write");
		}
	}

	if(write_file && S_ISREG(fs.st.st_mode)) {
		if((fd = open(file, O_RDONLY)) == -1) {
			call_error(file);
		}
		file_copy(fd, tfd, fs.st.st_size);
		close(fd);
	}
	else if(write_file && S_ISLNK(fs.st.st_mode)) {
		if(write(tfd, my_readlink(file), fs.st.st_size) != fs.st.st_size) {
			call_error("write");
		}
	}
	else if(S_ISDIR(fs.st.st_mode)) {
		file_buf = (char *)malloc(sizeof(char) * (_POSIX_PATH_MAX +1));
		if((dir = opendir(file)) == NULL) {
			call_error(file);
		}
		while((d_ent = readdir(dir)) != NULL) {
			if(strcmp(d_ent->d_name, ".") && strcmp(d_ent->d_name, "..")) {
				sprintf(file_buf, "%s/%s", remove_last_slashes(file), d_ent->d_name);
				add_file_to_tarball(file_buf, tfd, tar_path);
			}
		}
		closedir(dir);
		free(file_buf);
	}
	else if(S_ISCHR(fs.st.st_mode)) {
	}
	else if(S_ISBLK(fs.st.st_mode)) {
	}
	else if(S_ISFIFO(fs.st.st_mode)) {
	}
	else if(S_ISSOCK(fs.st.st_mode)) {
	}
}

int create_tarball(const char *tar_path, int argc, char **argv)
{
	int fd;

	if(tar_path) {
		if(tar_cmd == TAR_UPDATE || tar_cmd == TAR_APPEND) {
			fd = open(tar_path, O_WRONLY| O_APPEND| O_CREAT, 0664);
		} else {
			fd = open(tar_path, O_WRONLY| O_TRUNC| O_CREAT, 0664);
		}
		if(fd == -1)
			call_error(tar_path);
	}
	else fd = STDOUT_FILENO; // Else output to STDOUT

	while(argc-- > 0) {
		add_file_to_tarball(*(argv++), fd, tar_path);
	}

	if(fd > 2) close(fd);
}
