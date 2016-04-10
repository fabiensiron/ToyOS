USER_CFLAGS   = -O3 -m32 -Wa,--32 -g -Iuserspace -std=c99 -U__STRICT_ANSI__

FILES = ioapi.c zip.c minizip.c miniunz.c unzip.c

all: $(FILES)
	i686-pc-toyos-gcc $(USER_CFLAGS) ioapi.c zip.c minizip.c -o minizip -lz
	i686-pc-toyos-gcc $(USER_CFLAGS) ioapi.c unzip.c miniunz.c -o miniunz -lz

install: all
	cp minizip miniunz ../../hdd/usr/bin
