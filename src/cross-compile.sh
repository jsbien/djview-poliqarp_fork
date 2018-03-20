#!/bin/bash
# This script cross-compiles djview4poliqarp with a toolchain prepared with MXE (http://mxe.cc/)
# USAGE:
#  * to override MXE path, or use an existing instance, set env variable MXE_PATH (default: ./mxe)
#  * to override MXE target toolchain, set env variable MXE_TARGET (e.g. to build 32bit or shared
#    libs, default: x86_64-w64-mingw32.static)
#  * to set the number of parallel make jobs, set env variable MAKE_JOBS (default: 6)
#  * to skip MXE build, or use a custom cross-compile toolchain, set env variable NO_MXE_BUILD.
#    The path $MXE_PATH/usr/bin, or any directory on $PATH, should then contain the cross-compile
#    binaries, named $MXE_TARGET-qmake-qt5, $MXE_TARGET-g++, etc.


# exit on error
set -e
# output commands to the terminal
set -x

# set configuration
export MXE_PATH="${MXE_PATH:-$PWD/mxe}"
export MXE_TARGET="${MXE_TARGET:-x86_64-w64-mingw32.static}"
export MAKE_JOBS="${MAKE_JOBS:-6}"
SRC_DIR="$PWD"

if [ -z "$NO_MXE_BUILD" ]; then
    # get mxe if not present
    if [ ! -e "$MXE_PATH" ]; then
        git clone https://github.com/mxe/mxe.git "$MXE_PATH"
    fi

    # prepare the cross-compile toolchain
    cd "$MXE_PATH"
    make qtbase MXE_TARGETS=$MXE_TARGET -j$MAKE_JOBS
    make djvulibre MXE_TARGETS=$MXE_TARGET -j$MAKE_JOBS
fi

# build djview4poliqarp
export PATH="$MXE_PATH/usr/bin:$PATH"
cd "$SRC_DIR"
$MXE_TARGET-qmake-qt5
make -j$MAKE_JOBS
