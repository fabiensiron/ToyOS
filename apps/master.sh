#!/bin/bash

###################################################
# ToyOS Master Apps scripts                       #
# Change the following four variables             #
# Remember to update the build step at the bottom #
###################################################

# Program friendly name
PROGRAM_NAME="name"
# Program name with version in format name-x.x.x
PROGRAM_FULL_NAME="name-x.x.x"
# Program web link folder
PROGRAM_SOURCE="http://ftp.gnu.org/gnu/"
# Program name in source
PROGRAM_PACKAGE="name-x.x.x.tar.gz"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

. ../util/config.sh
. ../util/util.sh

if [ ! -d tarball ]; then
    mkdir tarball
fi

pushd tarball > /dev/null

    grab $PROGRAM_NAME $PROGRAM_SOURCE $PROGRAM_PACKAGE || bail
    deco $PROGRAM_NAME $PROGRAM_FULL_NAME || bail
    patc $PROGRAM_NAME $PROGRAM_FULL_NAME || bail

popd > /dev/null

if [ ! -d build ]; then
    mkdir build
fi

###################################################
# CHANGE ME!!!!                                   #
# Change the configure steps as appropriate       #
# You shouldn't have to touch the make steps      #
###################################################

pushd build > /dev/null
    ../$PROGRAM_FULL_NAME/configure --prefix=$VIRTPREFIX --host=$TARGET || bail
    make || bail
    make DESTDIR=$TOYOS_SYSROOT install || bail
popd
