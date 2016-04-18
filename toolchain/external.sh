#!/bin/bash
DIR=$(mktemp -d)
PWD=$(pwd)

echo "Building external ToyOS utilities..."
echo $DIR
cd $DIR
git clone https://github.com/PoisonNinja/star.git
cd star
make CC=i686-pc-toyos-gcc AR=i686-pc-toyos-ar INSTALL=$TOYOS_SYSROOT install
cd $PWD
rm $DIR -rf
echo "Done"
