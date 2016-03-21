#	Eudyptula Challenge Task 1
#	Makefile for countsensor.c
#	This Makefile was based on "The Linux Kernel Module Programming Guide"
#	http://www.tldp.org/LDP/lkmpg/2.6/html/lkmpg.html

#KDIR ?=/lib/modules/$(shell uname -r)/build 
export ARCH=arm
export CROSS_COMPILE=/home/prjs/toolchain/gcc-linaro/bin/arm-linux-gnueabihf-
KDIR ?=/home/prjs/toradex-linux-v2.5/kernel-src/colibri-vf-kernel-v2.5

obj-m += countsensor.o

all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
