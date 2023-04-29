ifneq ($(KERNELRELEASE),)
	obj-m := module_param.o
else
	KDIR := /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -f *.ko *.o *.symvers *.order *.mod.o *.mod.c
endif
