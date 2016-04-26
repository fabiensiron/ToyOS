if [ -z "$TOOLCHAIN" ]
then
    echo "Please build or activate the toolchain first"
    exit 1
fi

INFO=../util/mk-info

BUILD_FREETYPE=${BUILD_FREETYPE:-true}
BUILD_ZLIB=${BUILD_ZLIB:-true}
BUILD_PNG=${BUILD_PNG:-true}
BUILD_PIXMAN=${BUILD_PIXMAN:-true}
BUILD_CAIRO=${BUILD_CAIRO:-true}
BUILD_MESA=${BUILD_MESA:-true}
BUILD_NCURSES=${BUILD_NCURSES:-true}
BUILD_VIM=${BUILD_VIM:-true}
BUILD_NANO=${BUILD_NANO:-true}

function build () {
    $INFO "Building $1"
    pushd $1 > /dev/null
        ./pkg.sh
    popd > /dev/null
    $INFO "$1 built"
}

if $BUILD_FREETYPE; then
    build freetype
fi

if $BUILD_ZLIB; then
    build zlib
fi

if $BUILD_PNG; then
    build libpng
fi

if $BUILD_PIXMAN; then
    build pixman
fi

if $BUILD_CAIRO; then
    build cairo
fi

if $BUILD_MESA; then
    build mesa
fi

if $BUILD_NCURSES; then
    build ncurses
fi

if $BUILD_VIM; then
    build vim
fi

if $BUILD_NANO; then
    build nano
fi
