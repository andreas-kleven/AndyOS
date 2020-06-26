#!/bin/bash
parent_path=$(cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P)
srcdir=$parent_path/src

export PREFIX="$parent_path/build/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

cd $srcdir
 
mkdir build-binutils-cross
cd build-binutils-cross
../binutils-2.30/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make -j6
make -j6 install

cd $srcdir/gcc-8.1.0
./contrib/download_prerequisites

cd $srcdir
mkdir build-gcc-cross
cd build-gcc-cross
../gcc-8.1.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make -j6 all-gcc
make -j6 all-target-libgcc
make -j6 install-gcc
make -j6 install-target-libgcc

mkdir -p $HOME/opt/cross
cp -R $PREFIX/* $HOME/opt/cross
