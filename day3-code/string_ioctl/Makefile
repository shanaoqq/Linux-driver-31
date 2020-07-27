MODNAME?=myled
#KERNELDIR:= /lib/modules/$(shell uname -r)/build/
KERNELDIR:= /home/linux/kernel/kernel-3.4.39/
PWD:=$(shell pwd)

all:
	make -C $(KERNELDIR) M=$(PWD) modules
	arm-none-linux-gnueabi-gcc test.c -o APP-myled
	 cp *.ko APP-myled ~/rootfs/
clean:
	make -C $(KERNELDIR) M=$(PWD) clean

obj-m:=$(MODNAME).o
