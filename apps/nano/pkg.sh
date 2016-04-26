#!/bin/bash

# Program friendly name
PROGRAM_NAME="nano"
# Program name with version in format name-x.x.x
PROGRAM_FULL_NAME="nano-2.4.2"
# Program web link folder
PROGRAM_SOURCE="http://ftp.gnu.org/gnu/nano"
# Program name in source
PROGRAM_PACKAGE="nano-2.4.2.tar.gz"

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
    ../tarball/$PROGRAM_FULL_NAME/configure --prefix=$VIRTPREFIX --host=$TARGET --enable-tiny || bail
    make || bail
    make DESTDIR=$TOYOS_SYSROOT install || bail
popd

rm -rf build tarball
