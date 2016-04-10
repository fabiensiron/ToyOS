#include <stdio.h>
#include <sys/stat.h>
#include <syscall.h>
#include <stdint.h>
#include <sys/time.h>

int main(int argc, char ** argv) {
	int dereference = 0;
	if (argc < 2) {
		fprintf(stderr,"%s: expected argument\n", argv[0]);
		return 1;
	}

	char * file = argv[1];

	if (argc > 2) {
		if (!strcmp(argv[1],"-L")) {
			dereference = 1;
		}
		file = argv[2];
	}

	struct stat _stat;
	if (dereference) {
		if (stat(file, &_stat) < 0) return 1;
	} else {
		if (lstat(file, &_stat) < 0) return 1;
	}

	struct stat * f = &_stat;

	printf("st_mode  0x%x %d\n", (uint32_t)f->st_mode  , sizeof(f->st_mode  ));

	char permissions[10];

    permissions[0] = (f->st_mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (f->st_mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (f->st_mode & S_IXUSR) ? 'x' : '-';

    permissions[3] = (f->st_mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (f->st_mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (f->st_mode & S_IXGRP) ? 'x' : '-';

    permissions[6] = (f->st_mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (f->st_mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (f->st_mode & S_IXOTH) ? 'x' : '-';

    permissions[9] = '\0';

    printf("Permissions: %s\n", permissions);

	return 0;
}
