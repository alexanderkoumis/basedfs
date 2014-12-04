#!/bin/bash
sudo umount /media/mount
sudo rmmod basedfs
make clean
make
sudo insmod ../bin/basedfs.ko
sudo mount -o loop -t basedfs image /media/mount
dmesg
sudo cat /etc/mtab

