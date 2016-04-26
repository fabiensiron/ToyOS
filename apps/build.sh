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

if $BUILD_FREETYPE; then
    $INFO "Building freetype"
    pushd freetype > /dev/null
        ./pkg.sh
    popd > /dev/null
    $INFO "freetype built"
fi

if $BUILD_ZLIB; then
    $INFO "Building zlib"
    # TODO
    $INFO "zlib built"
fi

if $BUILD_PNG; then
    $INFO "Building libpng"
    pushd libpng > /dev/null
        ./pkg.sh
    popd > /dev/null
    $INFO "libpng built"
fi

if $BUILD_PIXMAN; then
    $INFO "Building pixman"
    pushd pixman > /dev/null
        ./pkg.sh
    popd > /dev/null
    $INFO "pixman built"
fi

if $BUILD_CAIRO; then
    $INFO "Building cairo"
    pushd cairo > /dev/null
        ./pkg.sh
    popd > /dev/null
    $INFO "cairo built"
fi

if $BUILD_MESA; then
    $INFO "Building mesa"
    pushd mesa > /dev/null
        ./pkg.sh
    popd > /dev/null
    $INFO "mesa built"
fi

if $BUILD_NCURSES; then
    $INFO "Building ncurses"
    pushd ncurses > /dev/null
        ./pkg.sh
    popd > /dev/null
    $INFO "ncurses built"
fi

if $BUILD_VIM; then
    $INFO "Building vim"
    pushd vim > /dev/null
        ./pkg.sh
    popd > /dev/null
    $INFO "vim built"
fi

if $BUILD_NANO; then
    $INFO "Building nano"
    pushd nano > /dev/null
        ./pkg.sh
    popd > /dev/null
    $INFO "nano built"
fi
