
.PHONY: all clean

ifneq ($(KERNELRELEASE),)
obj-m := hello.o
else
EXTRA_CFLAGS += -DDEBUG
KDIR := /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean

endif