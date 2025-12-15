
# KERNELDIR := /home/user/linux-4.15
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

obj-m := my_timer_module.o

all:
	make -C $(KERNELDIR) M=$(PWD) modules
clean:
	make -C $(KERNELDIR) M=$(PWD) clean
