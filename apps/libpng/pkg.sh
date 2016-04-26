#!/bin/bash

# Program friendly name
PROGRAM_NAME="libpng"
# Program name with version in format name-x.x.x
PROGRAM_FULL_NAME="libpng-1.5.13"
# Program web link folder
PROGRAM_SOURCE="http://b.dakko.us/~klange/mirrors"
# Program name in source
PROGRAM_PACKAGE="libpng-1.5.13.tar.gz"

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
    ../tarball/libpng-1.5.13/configure --host=$TARGET --prefix=$VIRTPREFIX || bail
    make || bail
    make DESTDIR=$TOYOS_SYSROOT install || bail
popd

rm -rf build tarball
