#!/bin/bash

gnome-terminal -x qemu-system-aarch64 -M raspi3 -kernel ../Build/kernel8.img -serial null -serial stdio -s -S

sleep 0.1

args=
for arg in "$@";
do
  args="$args '$arg'"
done

eval exec /usr/bin/aarch64-linux-gnu-gdb $args
