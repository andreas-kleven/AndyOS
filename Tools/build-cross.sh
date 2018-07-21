#!/bin/bash
parent_path=$(cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P)
srcdir=$parent_path/src

export PREFIX="$parent_path/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

cd $srcdir
 
mkdir build-binutils
cd build-binutils
../binutils-2.30/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install

cd $srcdir
 
# The $PREFIX/bin dir _must_ be in the PATH. We did that above.
which -- $TARGET-as || echo $TARGET-as is not in the PATH
 
mkdir build-gcc
cd build-gcc
../gcc-8.1.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc