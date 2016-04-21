#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char * name;

void usage(void) {
    fprintf(stderr, "usage: %s\n", name);
    exit(1);
}

int main(int argc, char ** argv) {
    name = argv[0];
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            usage();
        }
    }
    return system("uname -m");
}
