#!/bin/bash

# Program friendly name
PROGRAM_NAME="mesa"
# Program name with version in format name-x.x.x
PROGRAM_FULL_NAME="Mesa-7.5.2"
# Program web link folder
PROGRAM_SOURCE="http://b.dakko.us/~klange/mirrors"
# Program name in source
PROGRAM_PACKAGE="MesaLib-7.5.2.tar.gz"

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

pushd tarball/$PROGRAM_FULL_NAME > /dev/null
    ./configure --enable-32-bit --host=$TARGET --prefix=$VIRTPREFIX  --with-osmesa-bits=8 --with-driver=osmesa --disable-egl --disable-shared --without-x --disable-glw --disable-glut --disable-driglx-direct --disable-gallium || bail
    make || bail
    make DESTDIR=$TOYOS_SYSROOT install || bail
popd > /dev/null

rm -rf tarball
