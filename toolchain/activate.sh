#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. $DIR/config.sh

export PATH="$DIR/local/bin:$PATH"
export PKG_CONFIG_LIBDIR="$TOYOS_SYSROOT/usr/lib/pkgconfig"
export PKG_CONFIG_SYSROOT_DIR="$TOYOS_SYSROOT"
export TOOLCHAIN="$TOYOS_SYSROOT/usr"
