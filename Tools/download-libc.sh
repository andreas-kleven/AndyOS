#!/bin/bash
mkdir src
cd src
wget -nc ftp://sourceware.org/pub/newlib/newlib-3.3.0.tar.gz
wget -nc https://ftp.gnu.org/gnu/automake/automake-1.12.1.tar.gz
wget -nc https://ftp.gnu.org/gnu/autoconf/autoconf-2.69.tar.gz
tar xvf newlib-3.3.0.tar.gz
tar xvf automake-1.12.1.tar.gz
tar xvf autoconf-2.69.tar.gz
