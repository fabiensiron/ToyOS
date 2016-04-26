#!/bin/bash

# Program friendly name
PROGRAM_NAME="pixman"
# Program name with version in format name-x.x.x
PROGRAM_FULL_NAME="pixman-0.26.2"
# Location of the package (not including package)
PROGRAM_SOURCE="http://www.cairographics.org/releases"
# Program name in source
PROGRAM_PACKAGE="pixman-0.26.2.tar.gz"

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
    ../tarball/pixman-0.26.2/configure --host=$TARGET --prefix=$VIRTPREFIX || bail
    make || bail
    make DESTDIR=$TOYOS_SYSROOT install || bail
popd > /dev/null

rm -rf build tarball
