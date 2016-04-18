/* This file is part of ToyOS and is released under the terms
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

#include "lib/trace.h"

#define DEFAULT_HOSTNAME "toyos-test"

#define TRACE_APP_NAME "init"

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

const char* parseFile(char* line, int num) {
    const char* tok;
    for (tok = strtok(line, ":"); tok && *tok; tok = strtok(NULL, ":\n"))
    {
        // fprintf(stderr, "%s\n", tok);
        if (!--num)
            return tok;
    }
    return NULL;
}

int main(int argc, char * argv[]) {
	pid_t initPid = proc_find("init");
	if (initPid != -1) {
	   printf("%s\n", "init is already running!");
	   return 1;
	}
	/* stdin/out/err */
	set_console();
	/* Hostname */
	set_hostname();
    int runlevel = 1;
	if (argc > 1) {
		char * args = NULL;
		// if (argc > 2) {
		// 	args = argv[2];
		// }
        for (int i = 1; i < argc; i++) {
            if (!strcmp(argv[i], "3")) {
                return start_options((char *[]){"/bin/compositor","--","/bin/terminal","-Fl", args, NULL});
            } else if (!strcmp(argv[i], "1")) {
                return start_options((char *[]){"/bin/terminal-vga","-l",NULL});
            } else {
                /* Pass it to the compositor... */
                return start_options((char *[]){"/bin/compositor","--", args, NULL});
            }
        }
	}
    // No arguments! Read default inittab
    FILE* inittab = fopen("/etc/inittab", "r");
    char line[512];
    while (fgets(line, 512, inittab)) {
        char* tmp = strdup(line);
        // fprintf(stderr, "%s\n", (strcmp(parseFile(tmp, 3), "initdefault") == 0) ? "match" : "no match");
        if (strcmp(parseFile(tmp, 3), "initdefault") == 0) {
            tmp = strdup(line);
            runlevel = atoi(parseFile(tmp, 2));
        }
        fprintf(stderr, "%d\n", runlevel);
    }
    if (runlevel == 5) {
       return start_options((char *[]){"/bin/compositor",NULL});
    } else if (runlevel == 3) {
        return start_options((char *[]){"/bin/compositor","--","/bin/terminal","-Fl", NULL});
    } else {
        // Emergency mode
        return start_options((char *[]){"/bin/terminal-vga","-l",NULL});
    }
}
