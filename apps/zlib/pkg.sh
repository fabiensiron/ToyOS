#!/bin/bash

# Program friendly name
PROGRAM_NAME="zlib"
# Program name with version in format name-x.x.x
PROGRAM_FULL_NAME="zlib-1.2.8"
# Program web link folder
PROGRAM_SOURCE="http://zlib.net"
# Program name in source
PROGRAM_PACKAGE="zlib-1.2.8.tar.gz"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

. ../util/config.sh
. ../util/util.sh

if [ ! -d tarball ]; then
    mkdir tarball
fi

pushd tarball > /dev/null

    grab $PROGRAM_NAME $PROGRAM_SOURCE $PROGRAM_PACKAGE || bail
    deco $PROGRAM_NAME $PROGRAM_PACKAGE || bail

popd > /dev/null

pushd tarball/$PROGRAM_FULL_NAME > /dev/null
    CC=i686-pc-toyos-gcc ./configure --static --prefix=$VIRTPREFIX || bail
    make || bail
    make DESTDIR=$TOYOS_SYSROOT install || bail
popd > /dev/null

rm -rf build tarball
