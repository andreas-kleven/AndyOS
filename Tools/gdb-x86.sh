#!/bin/bash

GNOME_TERMINAL_SCREEN=""
gnome-terminal -- qemu-system-i386 -cdrom ../AndyOS.iso -s -S

sleep 0.1

args=
for arg in "$@";
do
  args="$args '$arg'"
done

eval exec gdb $args
