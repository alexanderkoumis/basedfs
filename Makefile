obj-m := basedfs.o
<<<<<<< HEAD
simplefs-objs := based.o

CFLAGS_simple.o := -DDEBUG
=======
>>>>>>> master

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
<<<<<<< HEAD
	make -C /lib/modules/$(shell uname -)/build M=$(PWD) clean
=======
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
>>>>>>> master
