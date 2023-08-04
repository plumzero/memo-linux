
ARCH=x86

PWD := $(shell pwd) 

TARGET = myusb.o

ifneq ($(KERNELRELEASE),)
obj-m := $(TARGET)
else

KERNEL_DIR = /path/to/linux-4.4.49
PWD := $(shell pwd)

all:
	make -C $(KERNEL_DIR) ARCH=$(ARCH) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean

endif
