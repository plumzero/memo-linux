
EXTRA_CFLAGS += -I/home/libei/Desktop/bak/linux/linux-4.4.49/drivers/md

KERNELDIR := /home/libei/Desktop/bak/linux/linux-4.4.49
PWD := $(shell pwd)

obj-m := dm-verity.o
obj-m += dm-bufio.o

all:
	make -C $(KERNELDIR) M=$(PWD) modules
clean:
	make -C $(KERNELDIR) M=$(PWD) clean
