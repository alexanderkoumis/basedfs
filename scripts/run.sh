#!/bin/bash
ROOT=~/git/cmpe142fs
if [ ! -f ~/mount -o ~/fs ]; then
    mkdir ~/mount ~/fs
fi
if [ ! -f $ROOT/src/image ]; then
    dd bs=8M count=16 if=/dev/zero of=$ROOT/src/image
fi
sudo umount -l ~/mount
sudo rmmod basedfs
make clean
make
sudo insmod $ROOT/bin/basedfs.ko
sudo mount -o loop -t basedfs $ROOT/src/image ~/mount
