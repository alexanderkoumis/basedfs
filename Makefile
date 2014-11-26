obj-m := basedfs.o
simplefs-objs := based.o

CFLAGS_simple.o := -DDEBUG

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -)/build M=$(PWD) clean
