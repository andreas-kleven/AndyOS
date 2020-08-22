#!/bin/bash

GNOME_TERMINAL_SCREEN=""
mkfifo /tmp/guest
gnome-terminal -- qemu-system-i386 -cdrom ../AndyOS.iso -drive file="/home/andreas/VirtualBox VMs/AndyOS/disk.img",media=cdrom -s -S -serial pipe:/tmp/guest -netdev bridge,id=mynet0 -device e1000,netdev=mynet0,mac=52:55:00:d1:55:01,id=nic1

sleep 0.1

args=
for arg in "$@";
do
  args="$args '$arg'"
done

eval exec gdb $args
