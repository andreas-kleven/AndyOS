#!/bin/bash
mkdir src
cd src
wget ftp://sourceware.org/pub/newlib/newlib-3.3.0.tar.gz -O - | tar -xz
wget https://ftp.gnu.org/gnu/automake/automake-1.12.1.tar.gz -O - | tar -xz
wget https://ftp.gnu.org/gnu/autoconf/autoconf-2.69.tar.gz -O - | tar -xz
