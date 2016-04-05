/* This file is part of ToaruOS and is released under the terms
 * of the NCSA / University of Illinois License - see LICENSE.md
 * Copyright (C) 2013-2014 Kevin Lange
 */
/* vim: tabstop=4 shiftwidth=4 noexpandtab
 *
 * init
 *
 * Provides the standard boot routines and
 * calls the user session (compositor / terminal)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <syscall.h>
#include <dirent.h>
#include <sys/wait.h>

#define DEFAULT_HOSTNAME "toyos-test"


pid_t proc_find(const char* name)
{
    DIR* dir;
    struct dirent* ent;
    char* endptr;
    char buf[512];

    if (!(dir = opendir("/proc"))) {
        perror("can't open /proc");
        return -1;
    }

    while((ent = readdir(dir)) != NULL) {
        /* if endptr is not a null character, the directory is not
         * entirely numeric, so ignore it */
        long lpid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0') {
            continue;
        }

        /* try to open the cmdline file */
        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
        FILE* fp = fopen(buf, "r");

        if (fp) {
            if (fgets(buf, sizeof(buf), fp) != NULL) {
                /* check the first token in the file, the program name */
                char* first = strtok(buf, " ");
                if (!strcmp(first, name)) {
                    fclose(fp);
                    closedir(dir);
                    return (pid_t)lpid;
                }
            }
            fclose(fp);
        }

    }

    closedir(dir);
    return -1;
}

void set_console() {
	int _stdin  = open("/dev/null", O_RDONLY);
	int _stdout = open("/dev/ttyS0", O_WRONLY);
	int _stderr = open("/dev/ttyS0", O_WRONLY);

	if (_stdout < 0) {
		_stdout = open("/dev/null", O_WRONLY);
		_stderr = open("/dev/null", O_WRONLY);
	}
}

/* Set the hostname to whatever is in /etc/hostname */
void set_hostname() {
	FILE * _host_file = fopen("/etc/hostname", "r");
	if (!_host_file) {
		/* No /etc/hostname, use the default */
		syscall_sethostname(DEFAULT_HOSTNAME);
	} else {
		char buf[256];
		fgets(buf, 255, _host_file);
		if (buf[strlen(buf)-1] == '\n') {
			buf[strlen(buf)-1] = '\0';
		}
		syscall_sethostname(buf);
		setenv("HOST", buf, 1);
		fclose(_host_file);
	}
}

int start_options(char * args[]) {
	pid_t initPid = proc_find("init");
	if (initPid != -1) {
	   printf("%s\n", "init is already running!");
	   return 1;
	}
	int pid = fork();
	if (!pid) {
		int i = execvp(args[0], args);
		exit(0);
	} else {
		int pid = 0;
		do {
			pid = wait(NULL);
		} while ((pid > 0) || (pid == -1 && errno == EINTR));
	}
}

int main(int argc, char * argv[]) {
	/* stdin/out/err */
	set_console();
	/* Hostname */
	set_hostname();
	if (argc > 1) {
		char * args = NULL;
		if (argc > 2) {
			args = argv[2];
		}
		if (!strcmp(argv[1],"--single")) {
			return start_options((char *[]){"/bin/compositor","--","/bin/terminal","-Fl",args,NULL});
		} else if (!strcmp(argv[1], "--vga")) {
			return start_options((char *[]){"/bin/terminal-vga","-l",NULL});
		} else {
			/* Pass it to the compositor... */
			return start_options((char *[]){"/bin/compositor","--",argv[1],NULL});
		}
	}
	return start_options((char *[]){"/bin/compositor",NULL});
}
