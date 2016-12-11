ifneq ($(KERNELRELEASE),)
#we zitten in de kerneltree
obj-m := procdriver.o 

else
#we zitten niet in de kerneltree
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default: 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules 
	rm -r -f .tmp_versions *.mod.c .*.cmd *.o *.symvers 
endif 