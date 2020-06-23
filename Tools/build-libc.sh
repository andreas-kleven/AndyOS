#!/bin/bash
SYSTEM=andyos
SYSROOT=$(realpath ~/$SYSTEM)

parent_path=$(cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P)
srcdir=$parent_path/src
bindir=$parent_path/bin
syssrc=$parent_path/../Programs/Libc

mkdir $bindir
mkdir build
cd build

$srcdir/automake-1.12.1/configure --prefix="$bindir"
make && make install

$srcdir/autoconf-2.69/configure --prefix="$bindir"
make && make install

bindir=$bindir/bin
export PATH=$bindir:$PATH

cd $srcdir/newlib-3.0.0/
if ! grep -q $SYSTEM "config.sub"; then
    awk "/is not here because it comes later, after sysvr4./ { print; print \"    -$SYSTEM* | \\\\\"; next }1" config.sub > config.sub.tmp
    mv config.sub.tmp config.sub
fi

cd newlib
if ! grep -q $SYSTEM "configure.host"; then
    awk "/Get the source directories to use for the host./,/case/ { if(\$0 ~ /case/) { print; print \"  i[3-7]86-*-$SYSTEM*)\"; print \"    sys_dir=$SYSTEM\"; print \"    ;;\"; next }}1" configure.host > configure.host.tmp
    mv configure.host.tmp configure.host
fi

cd libc/sys
if ! grep -q $SYSTEM "configure.in"; then
    awk "/case/ { print; print \"    $SYSTEM) AC_CONFIG_SUBDIRS($SYSTEM) ;;\"; next }1" configure.in > configure.in.tmp
    mv configure.in.tmp configure.in
fi

mkdir $SYSTEM
cd $SYSTEM
cp $syssrc/* .
cp $parent_path/../Kernel/Include/syscall_list.h .

aclocal -I ../../..
automake --cygnus Makefile

cd ..
autoconf
cd $SYSTEM
autoconf

ln -s $(which i686-elf-ar) $bindir/i686-$SYSTEM-ar
ln -s $(which i686-elf-as) $bindir/i686-$SYSTEM-as
ln -s $(which i686-elf-gcc) $bindir/i686-$SYSTEM-gcc
ln -s $(which i686-elf-gcc) $bindir/i686-$SYSTEM-cc
ln -s $(which i686-elf-ranlib) $bindir/i686-$SYSTEM-ranlib

cd $parent_path
mkdir build-newlib
cd build-newlib

$srcdir/newlib-3.0.0/configure --prefix=/usr --target=i686-$SYSTEM
make all
make DESTDIR=${SYSROOT} install
