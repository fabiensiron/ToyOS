#!/bin/bash

# Program friendly name
PROGRAM_NAME="cairo"
# Program name with version in format name-x.x.x
PROGRAM_FULL_NAME="cairo-1.12.2"
# Program web link folder
PROGRAM_SOURCE="http://www.cairographics.org/releases"
# Program name in source
PROGRAM_PACKAGE="cairo-1.12.2.tar.xz"

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
    ../tarball/$PROGRAM_FULL_NAME/configure --host=$TARGET --prefix=$VIRTPREFIX --enable-ps=no --enable-pdf=no --enable-interpreter=no --enable-xlib=no || bail
    cp ../cairo-Makefile test/Makefile
    cp ../cairo-Makefile perf/Makefile
    echo -e "\n\n#define CAIRO_NO_MUTEX 1" >> config.h
    make || bail
    make DESTDIR=$TOYOS_SYSROOT install || bail
popd

rm -rf build tarball
