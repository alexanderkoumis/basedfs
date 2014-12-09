ifneq ($(KERNELRELEASE),)
obj-m := ../bin/basedfs.o
../bin/basedfs-y := basedfs.o
basedfs-y := basedfs.c

CFLAGS_basedfs.o = -DDEBUG
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

else
KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD modules

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

endif
