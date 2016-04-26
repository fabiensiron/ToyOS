#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

. $DIR/config.sh
. $DIR/util.sh

# Build everything by default.
BUILD_BINUTILS=true
BUILD_GCC=true
BUILD_NEWLIB=true
BUILD_LIBSTDCPP=true

# BUILD_BINUTILS=false
# BUILD_GCC=false
# BUILD_NEWLIB=false
# BUILD_LIBSTDCPP=false

echo "Building a toolchain with a sysroot of $TOYOS_SYSROOT with host binaries in $PREFIX targeting $TARGET"

if [ ! -d build ]; then
    mkdir build
fi

pushd build
    if $BUILD_BINUTILS; then
        if [ ! -d binutils ]; then
            mkdir binutils
        fi

        unset PKG_CONFIG_LIBDIR

        pushd binutils
            $DIR/tarballs/binutils-2.22/configure --target=$TARGET --prefix=$PREFIX --with-sysroot=$TOYOS_SYSROOT --disable-werror || bail
            make || bail
            make install || bail
        popd
    fi

    if $BUILD_GCC; then
        if [ -d gcc ]; then
            rm -rf gcc
        fi
        mkdir gcc

        unset PKG_CONFIG_LIBDIR

        pushd gcc
            $DIR/tarballs/gcc-4.6.4/configure --target=$TARGET --prefix=$PREFIX --with-sysroot=$TOYOS_SYSROOT --with-build-sysroot=$TOYOS_SYSROOT --with-native-system-header-dir=$TOYOS_SYSROOT --disable-nls --enable-languages=c,c++ --disable-libssp --with-newlib || bail
            make all-gcc || bail
            make install-gcc || bail
            make all-target-libgcc || bail
            make install-target-libgcc || bail
        popd
    fi

    . $DIR/activate.sh

    if $BUILD_NEWLIB; then
        if [ ! -d newlib ]; then
            mkdir newlib
        else
            # Newlib is touchy about reconfigures
            rm -r newlib
            mkdir newlib
        fi
        pushd $DIR/tarballs/newlib-2.4.0
            find -type f -exec sed 's|--cygnus||g;s|cygnus||g' -i {} + || bail
        popd
        pushd $DIR/tarballs/newlib-2.4.0/newlib/libc/sys
            autoconf || bail
            pushd toyos
                touch INSTALL NEWS README AUTHORS ChangeLog COPYING || bail
                autoreconf || bail
                yasm -f elf -o crt0.o crt0.s || bail
                yasm -f elf -o crti.o crti.s || bail
                yasm -f elf -o crtn.o crtn.s || bail
                cp crt0.o ../
                cp crt0.o /tmp/__toyos_crt0.o
                cp crti.o ../
                cp crti.o /tmp/__toyos_crti.o
                cp crtn.o ../
                cp crtn.o /tmp/__toyos_crtn.o
            popd
        popd
        pushd newlib
            mkdir -p $TARGET/newlib/libc/sys
            cp /tmp/__toyos_crt0.o $TARGET/newlib/libc/sys/crt0.o
            rm /tmp/__toyos_crt0.o
            cp /tmp/__toyos_crti.o $TARGET/newlib/libc/sys/crti.o
            rm /tmp/__toyos_crti.o
            cp /tmp/__toyos_crtn.o $TARGET/newlib/libc/sys/crtn.o
            rm /tmp/__toyos_crtn.o
            echo "" > $DIR/tarballs/newlib-2.4.0/newlib/libc/stdlib/malign.c
            $DIR/tarballs/newlib-2.4.0/configure --target=$TARGET --prefix=$VIRTPREFIX || bail
            # Fix the damned tooldir
            sed -s 's/prefix}\/i686-pc-toyos/prefix}/' Makefile > Makefile.tmp
            mv Makefile.tmp Makefile
            make || bail
            make DESTDIR=$TOYOS_SYSROOT install || bail
            cp -r $DIR/patches/newlib/include/* $TOYOS_SYSROOT/$VIRTPREFIX/include/
            cp $TARGET/newlib/libc/sys/crt0.o $TOYOS_SYSROOT/$VIRTPREFIX/lib/
            cp $TARGET/newlib/libc/sys/crti.o $TOYOS_SYSROOT/$VIRTPREFIX/lib/
            cp $TARGET/newlib/libc/sys/crtn.o $TOYOS_SYSROOT/$VIRTPREFIX/lib/
        popd
    fi

    if $BUILD_LIBSTDCPP; then
        pushd gcc
            # build libstdc++
            make all-target-libstdc++-v3 || bail
            make install-target-libstdc++-v3 || bail
        popd
    fi
    
    pushd $TOYOS_SYSROOT/usr/bin || bail
        $TARGET-strip *
    popd

popd
