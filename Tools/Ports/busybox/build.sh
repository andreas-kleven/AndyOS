#!/bin/bash
cd "$(dirname "$0")"

destdir=$PWD/../../../Root

git clone -b 1_32_stable --single-branch git://busybox.net/busybox.git src
cd src
git apply --ignore-space-change --ignore-whitespace ../patches.patch

cp ../.config ./
make busybox install
cp _install/bin/* $destdir/bin
cp _install/usr/bin/* $destdir/bin
