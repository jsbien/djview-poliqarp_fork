# Cross-compiling djview4poliqarp for Windows on Linux

## 1. Rationale

Compiling on Windows is not simple, requires installation of special IDEs,
manually moving files, etc. Cross-compilation allows to do it on your favourite
Linux distribution and automate the process.

Generally, to cross-compile this application, you need:

 * A cross-compile toolchain
 * cross-compiled [Qt](https://www.qt.io/download-open-source/) libraries
 * cross-compiled [djvulibre](http://djvu.sourceforge.net/) libraries
 * cross-compiled dependencies of the above

Creating and using cross-compile toolchain manually is not simple though. Some
distributions try to make it easier (like Gentoo's
[crossdev](https://wiki.gentoo.org/wiki/Cross_build_environment)), but in most
cases it still requires some manual configuration. In this project we use
[MXE](http://mxe.cc/), a Makefile for creating such toolchains, configured and
maintained by the community. A simple script is added to automate all the steps
into a single command.

## 2. Requirements

 * Basic Linux programs and build tools -- see specific
   [MXE requirements](http://mxe.cc/#requirements). You normally don't need
   to download MXE nor build its toolchains, it will be done automatically
   by the included script -- it just requires MXE requirements to be satisfied.
 * About 1.4GB disk space for the toolchain and libs
 * Some time, if building the toolchain (~37 minutes with 6 jobs on an i7
   processor)

## 3. Simple compilation

Install MXE requirements, if needed. Go to the `src` directory and run
`cross-compile.sh`. The default options download MXE and build the toolchain
automatically, then compile a statically linked 64-bit binary -- which is
a standalone executable and doesn't need any additional files to run. If you
didn't delete the MXE directory from a previous build, it should be used
automatically to save time.

The resulting binary will be located at `src/djview4poliqarp.exe`.

## 4. Advanced options

 * To change MXE path, or use an existing MXE directory, set the environment
   variable `MXE_PATH` (defaults to `./mxe`). For example, if you want to
   install the toolchain system-wide, set e.g. `MXE_PATH=/opt/mxe` and run
   the script with root privileges.
 * To change the MXE target toolchain, for example to use 32bit or shared
   libraries, set the environment variable `MXE_TARGET` (defaults to
   `x86_64-w64-mingw32.static`). Using different target you may have to adjust
   the additional cross-compilation options in the `win32` section of the `.pro`
   file.
 * To significantly shorten the build time, the script uses `make` with parallel
   jobs. To change the maximum number of jobs, set the environment variable
   `MAKE_JOBS` (defaults to 6, which seems to be best for an 8-core processor).
 * To skip building the toolchain or use a custom one, set the env variable
   `NO_MXE_BUILD`. The path `$MXE_PATH/usr/bin`, or any directory on `$PATH`,
   should then contain the cross-compile binaries, named `$MXE_TARGET-qmake-qt5`,
   `$MXE_TARGET-g++`, etc.
