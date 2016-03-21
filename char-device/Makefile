#	Eudyptula Challenge Task 1
#	Makefile for helloLKM.c
#	This Makefile was based on "The Linux Kernel Module Programming Guide"
#	http://www.tldp.org/LDP/lkmpg/2.6/html/lkmpg.html

#KDIR ?=/lib/modules/$(shell uname -r)/build 
KDIR ?=/home/prjs/toradex-linux-v2.5/kernel-src/colibri-vf-kernel-v2.5
CROSS_TOOLCHAIN = /home/prjs/toolchain/gcc-linaro/bin/arm-linux-gnueabihf-gcc


obj-m += chardevice.o

all:
	make -C $(KDIR) M=$(PWD) modules
	$(CROSS_TOOLCHAIN) testchardevice.c -o testchardevice
clean:
	make -C $(KDIR) M=$(PWD) clean
