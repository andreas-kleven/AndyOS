#!/bin/bash
mkdir src
cd src
wget -nc https://ftp.gnu.org/gnu/automake/automake-1.11.6.tar.gz
wget -nc https://ftp.gnu.org/gnu/autoconf/autoconf-2.64.tar.gz
wget -nc https://ftp.gnu.org/gnu/binutils/binutils-2.30.tar.gz
wget -nc https://ftp.gnu.org/gnu/gcc/gcc-8.1.0/gcc-8.1.0.tar.gz
tar xvf automake-1.11.6.tar.gz
tar xvf autoconf-2.64.tar.gz
tar xvf binutils-2.30.tar.gz
tar xvf gcc-8.1.0.tar.gz
