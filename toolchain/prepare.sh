#!/bin/bash

# Toolchain Installer for Debian-like systems. If you're running
# something else, you're pretty much on your own.

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

. $DIR/util.sh

function deleteUnusedGCC () {
    # If you are running from the non-core GCC, run this function delete the stuff we don't care about
    rm -r $1/boehm-gc $1/gcc/ada $1/gcc/go $1/gcc/java $1/gcc/objc $1/gcc/objcp $1/gcc/testsuite $1/gnattools $1/libada $1/libffi $1/libgo $1/libjava $1/libobjc
}

pushd "$DIR" > /dev/null

    if [ ! -d tarballs ]; then
        mkdir tarballs
    fi
    pushd tarballs > /dev/null
        $INFO "wget" "Pulling source packages..."
        grab "gcc"  "http://www.netgull.com/gcc/releases/gcc-4.6.4" "gcc-core-4.6.4.tar.gz" || bail
        grab "g++"  "http://www.netgull.com/gcc/releases/gcc-4.6.4" "gcc-g++-4.6.4.tar.gz" || bail
        #grab "mpc"  "http://www.multiprecision.org/mpc/download" "mpc-0.9.tar.gz"
        #grab "mpfr" "http://www.mpfr.org/mpfr-3.0.1" "mpfr-3.0.1.tar.gz"
        #grab "gmp"  "ftp://ftp.gmplib.org/pub/gmp-5.0.1" "gmp-5.0.1.tar.gz"
        grab "binutils" "http://ftp.gnu.org/gnu/binutils" "binutils-2.22.tar.gz" || bail
        grab "newlib" "ftp://sourceware.org/pub/newlib" "newlib-2.4.0.tar.gz" || bail
        $INFO "wget" "Pulled source packages."
        rm -rf "binutils-2.22" "gcc-4.6.4" "gmp-5.0.1" "mpc-0.9" "mpfr-3.0.1" "newlib-2.4.0"
        $INFO "tar"  "Decompressing..."
        deco "gcc"  "gcc-core-4.6.4.tar.gz" || bail
        deco "g++"  "gcc-g++-4.6.4.tar.gz" || bail
        #deco "mpc"  "mpc-0.9.tar.gz"
        #deco "mpfr" "mpfr-3.0.1.tar.gz"
        #deco "gmp"  "gmp-5.0.1.tar.gz"
        deco "binutils" "binutils-2.22.tar.gz" || bail
        deco "newlib" "newlib-2.4.0.tar.gz" || bail
        $INFO "tar"  "Decompressed source packages."
        $INFO "patch" "Patching..."
        patc "gcc"  "gcc-4.6.4" || bail
        #patc "mpc"  "mpc-0.9"
        #patc "mpfr" "mpfr-3.0.1"
        #patc "gmp"  "gmp-5.0.1"
        patc "binutils" "binutils-2.22" || bail
        patc "newlib" "newlib-2.4.0" || bail
        $INFO "patch" "Patched third-party software."
        $INFO "--" "Running additional bits..."
        installNewlibStuff "newlib-2.4.0" || bail
    popd > /dev/null

    if [ ! -d build ]; then
        mkdir build
    fi
    if [ ! -d local ]; then
        mkdir local
    fi

popd > /dev/null
