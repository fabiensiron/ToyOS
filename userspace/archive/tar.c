#include "lib/libtar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tar_cmd;
int tar_flag;
char * name;

void help(const char *msg)
{
    if(msg) {
        fprintf(stderr, "ERROR: %s\n\n", msg);
    }
    fprintf(stderr,
            "USAGE: %s [OPTIONS] [FILE...]\n\n"
            "Where OPTIONS is a string containing one of the following:\n\n"
            "c     Create the archive\n"
            "x     Extract the archive\n"
            "t     Test the archive\n"
            "u     Update the archive\n\n"
            "Or additional flags:\n\n"
            "f     Input/Output from/to the specified file\n"
            "v     Verbose output\n"
    , name);

    exit(-1);
}

int parse_options(const char *opts)
{
    tar_flag = 0;
    tar_cmd = 0;

    while(*opts) {
        switch (*opts) {
            case 'x':
                if(tar_cmd) help(NULL);
                tar_cmd = TAR_EXTRACT;
                break;

            case 'c':
                if(tar_cmd) help(NULL);
                tar_cmd = TAR_CREATE;
                break;

            case 't':
                if(tar_cmd) help(NULL);
                tar_cmd = TAR_LIST;
                break;

            case 'u':
                if(tar_cmd) help(NULL);
                tar_cmd = TAR_UPDATE;
                break;

            case 'r':
                if(tar_cmd) help(NULL);
                tar_cmd = TAR_APPEND;
                break;

            case 'v':
                verbose = 1;
                break;

            case 'f':
                tar_flag |= TAR_FILE;
                break;

            case 'h':
                tar_flag |= TAR_FOLLOW_SYMLINKS;
                break;

            default :
                help("Unknown option");
        }
        opts++;
    }

    if(!tar_cmd) help(NULL);
    return 0;
}

int main(int argc, char **argv)
{
    name = strdup(argv[0]);
    char *tarball_name = NULL;

    if(argc < 2) {
        help(NULL);
    }

    parse_options(argv[1]);
    argc -= 2;
    argv += 2;

    if(tar_flag & TAR_FILE) {
        tarball_name = argv[0];
        argc--;
        argv++;
    }

    switch (tar_cmd)
    {
        case TAR_CREATE:
            create(tarball_name, argv, argc);
            break;
        case TAR_UPDATE:
            update(tarball_name, argv, argc);
            break;
        case TAR_APPEND:
            append(tarball_name, argv, argc);
            break;
        case TAR_EXTRACT:
            extract(tarball_name);
            break;
        case TAR_LIST:
            list(tarball_name);
            break;
    }
}
