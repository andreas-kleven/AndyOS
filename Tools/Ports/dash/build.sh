#!/bin/bash
cd "$(dirname "$0")"

builddir=$PWD/build
destdir=$PWD/../../../Root
mkdir $builddir

git clone git://git.kernel.org/pub/scm/utils/dash/dash.git src
cd src
git apply --ignore-space-change --ignore-whitespace ../patches.patch

./autogen.sh
./configure --host=i686-andyos --prefix=$builddir CFLAGS="-g -O0" LDFLAGS=-Wl,--allow-multiple-definition
make && make install

cp -R $builddir/* $destdir
