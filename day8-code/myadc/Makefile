MODNAME?=adc_irq
#KERNELDIR:= /lib/modules/$(shell uname -r)/build/
KERNELDIR:= /home/linux/kernel/kernel-3.4.39/
PWD:=$(shell pwd)

all:
	make -C $(KERNELDIR) M=$(PWD) modules
clean:
	make -C $(KERNELDIR) M=$(PWD) clean

obj-m:=$(MODNAME).o
