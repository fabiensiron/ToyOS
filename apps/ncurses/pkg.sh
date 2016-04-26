#!/bin/bash

# Program friendly name
PROGRAM_NAME="ncurses"
# Program name with version in format name-x.x.x
PROGRAM_FULL_NAME="ncurses-5.9"
# Program web link folder
PROGRAM_SOURCE="http://b.dakko.us/~klange/mirrors"
# Program name in source
PROGRAM_PACKAGE="ncurses-5.9.tar.gz"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

. ../util/config.sh
. ../util/util.sh

if [ ! -d tarball ]; then
    mkdir tarball
fi

pushd tarball > /dev/null

    grab $PROGRAM_NAME $PROGRAM_SOURCE $PROGRAM_PACKAGE || bail
    deco $PROGRAM_NAME $PROGRAM_PACKAGE || bail
    patc $PROGRAM_NAME $PROGRAM_FULL_NAME || bail

popd > /dev/null

if [ ! -d build ]; then
    mkdir build
fi

pushd build > /dev/null
    ../tarball/ncurses-5.9/configure --prefix=$VIRTPREFIX --host=$TARGET --with-terminfo-dirs=/usr/share/terminfo --with-default-terminfo-dir=/usr/share/terminfo --without-tests || bail
    make || bail
    make DESTDIR=$TOYOS_SYSROOT install || bail
    cp ../toyos.tic $TOYOS_SYSROOT/$VIRTPREFIX/share/terminfo/t/toyos
    cp ../toyos-vga.tic $TOYOS_SYSROOT/$VIRTPREFIX/share/terminfo/t/toyos-vga
popd > /dev/null

rm -rf build tarball
