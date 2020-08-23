#!/bin/bash
mkdir src
cd src
wget https://ftp.gnu.org/gnu/automake/automake-1.11.6.tar.gz -O - | tar -xz
wget https://ftp.gnu.org/gnu/autoconf/autoconf-2.64.tar.gz -O - | tar -xz
curl -s https://ftp.gnu.org/gnu/binutils/binutils-2.30.tar.xz | tar xvf - -J
curl -s https://ftp.gnu.org/gnu/gcc/gcc-8.1.0/gcc-8.1.0.tar.xz | tar xvf - -J
