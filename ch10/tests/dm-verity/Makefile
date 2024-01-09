
KERNELDIR := /path/to/linux-4.4.49
PWD := $(shell pwd)

EXTRA_CFLAGS += -I/path/to/linux-4.4.49/drivers/md

obj-m := dm-verity.o

all:
	make -C $(KERNELDIR) M=$(PWD) modules
clean:
	make -C $(KERNELDIR) M=$(PWD) clean
