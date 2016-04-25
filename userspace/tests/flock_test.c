#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>

int
main(int argc, char *argv[]) {
	// int fd = open("FLOCK_TEST", O_CREAT);
	//
	// int r = flock(fd, LOCK_EX | LOCK_NB);
	//
	// if (r == -1) {
	// 	printf("Waiting for free external unlock...\n");
	// 	while(flock(fd, LOCK_EX | LOCK_NB) == -1);
	// 	printf("Resource unlocked\n");
	// }
	//
	// printf("wait...\n");
	//
	// sleep(5);
	//
	// printf("unlock...\n");
	//
	// flock(fd, LOCK_UN | LOCK_NB);
	//
	// printf("done!\n");
	//
	// close(fd);
	//
	// return 0;
}
