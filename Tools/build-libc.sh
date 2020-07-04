#!/bin/bash
SYSTEM=andyos
SYSROOT=$HOME/$SYSTEM

parent_path=$(cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P)
srcdir=$parent_path/src
syssrc=$parent_path/../Programs/Libc

export PREFIX="$parent_path/build/libc"
export TARGET=i686-$SYSTEM
export PATH="$PREFIX/bin:$PATH"

cd $srcdir
mkdir build-automake-1.12.1
cd build-automake-1.12.1
$srcdir/automake-1.12.1/configure --prefix="$PREFIX"
make -j6 && make install

cd $srcdir
mkdir build-autoconf-2.69
cd build-autoconf-2.69
$srcdir/autoconf-2.69/configure --prefix="$PREFIX"
make -j6 && make install

# Newlib

cd $srcdir/newlib-3.0.0/
if ! grep -q $SYSTEM "config.sub"; then
    awk "/is not here because it comes later, after sysvr4./ { print; print \"    -$SYSTEM* | \\\\\"; next }1" config.sub > config.sub.tmp
    mv config.sub.tmp config.sub
fi

cd newlib
if ! grep -q $SYSTEM "configure.host"; then
    awk "/Get the source directories to use for the host./,/case/ { if(\$0 ~ /case/) { print; print \"  i[3-7]86-*-$SYSTEM*)\"; print \"    sys_dir=$SYSTEM\"; print \"    newlib_cflags=\\\"\${newlib_cflags} -DSIGNAL_PROVIDED\\\"\"; print \"    ;;\"; next }}1" configure.host > configure.host.tmp
    mv configure.host.tmp configure.host
fi

cd libc/sys
if ! grep -q $SYSTEM "configure.in"; then
    awk "/case/ { print; print \"    $SYSTEM) AC_CONFIG_SUBDIRS($SYSTEM) ;;\"; next }1" configure.in > configure.in.tmp
    mv configure.in.tmp configure.in
fi

mkdir $SYSTEM
cd $SYSTEM
cp -R $syssrc/* .
cp $parent_path/../Kernel/Include/syscall_list.h .

aclocal -I ../../..
automake --cygnus Makefile

cd ..
autoconf
cd $SYSTEM
autoconf

# Build

cd $srcdir
mkdir build-newlib-3.0.0
cd build-newlib-3.0.0
$srcdir/newlib-3.0.0/configure --target=$TARGET --prefix="$PREFIX"
make -j6 all
make -j6 install

mkdir -p $SYSROOT/usr
cp -R $PREFIX/$TARGET/* $SYSROOT/usr
cp -R $syssrc/arpa $SYSROOT/usr/include/arpa
