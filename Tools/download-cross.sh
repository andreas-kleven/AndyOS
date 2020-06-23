#!/bin/bash
mkdir src
cd src
curl -s https://ftp.gnu.org/gnu/bison/bison-3.0.5.tar.xz | tar xvf - -J
wget -O flex.tar.gz https://github.com/westes/flex/releases/download/v2.6.4/flex-2.6.4.tar.gz && tar xvzf flex.tar.gz && rm flex.tar.gz
curl -s https://ftp.gnu.org/gnu/gmp/gmp-6.1.2.tar.xz | tar xvf - -J
curl -s https://ftp.gnu.org/gnu/mpc/mpc-1.1.0.tar.gz | tar xvf - -J
curl -s https://ftp.gnu.org/gnu/mpfr/mpfr-4.0.1.tar.xz | tar xvf - -J
curl -s https://ftp.gnu.org/gnu/texinfo/texinfo-6.5.tar.xz | tar xvf - -J
curl -s https://ftp.gnu.org/gnu/binutils/binutils-2.30.tar.xz | tar xvf - -J
curl -s https://ftp.gnu.org/gnu/gcc/gcc-8.1.0/gcc-8.1.0.tar.xz | tar xvf - -J
