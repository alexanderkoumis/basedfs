obj-m := ../bin/basedfs.o
../bin/basedfs-y := basedfs.o
basedfs-y := basedfs.c
FLAGS=-std=gnu99
CFLAGS_basedfs.o = -DDEBUG
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

KDIR ?= /usr/src/linux-headers-3.13.0-36-generic

all:
	$(MAKE) -C $(KDIR) M=$$PWD modules
	gcc -o ../bin/socketprog udpSocket.c

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
	rm -rf src/*o
