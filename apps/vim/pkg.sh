#!/bin/bash

# Program friendly name
PROGRAM_NAME="vim"
# Program name with version in format name-x.x.x
PROGRAM_FULL_NAME="vim73"
# Program web link folder
PROGRAM_SOURCE="ftp://ftp.vim.org/pub/vim/unix"
# Program name in source
PROGRAM_PACKAGE="vim-7.3.tar.bz2"

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
    ac_cv_sizeof_int=4 vim_cv_getcwd_broken=no vim_cv_memmove_handles_overlap=yes vim_cv_stat_ignores_slash=no vim_cv_tgetent=zero vim_cv_terminfo=yes vim_cv_toupper_broken=no vim_cv_tty_group=world ./configure --host=$TARGET --prefix=$VIRTPREFIX --with-tlib=ncurses --enable-gui=no --disable-gtktest --disable-xim --with-features=normal --disable-gpm --without-x --disable-netbeans --enable-multibyte || bail
    make || bail
    make DESTDIR=$TOYOS_SYSROOT install || bail
popd

rm -rf tarball
