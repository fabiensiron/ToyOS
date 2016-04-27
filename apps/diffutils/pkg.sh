#!/bin/bash

# Program friendly name
PROGRAM_NAME="diffutils"
# Program name with version in format name-x.x.x
PROGRAM_FULL_NAME="diffutils-3.3"
# Program web link folder
PROGRAM_SOURCE="http://ftp.gnu.org/gnu/diffutils/"
# Program name in source
PROGRAM_PACKAGE="diffutils-3.3.tar.xz"

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
    ../tarball/$PROGRAM_FULL_NAME/configure --prefix=$VIRTPREFIX --host=$TARGET || bail
    make || bail
    make DESTDIR=$TOYOS_SYSROOT install || bail
popd

rm -rf build tarball
