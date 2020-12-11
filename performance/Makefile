MODULE	 = del

obj-m += $(MODULE).o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

all: $(MODULE)

%.o: %.c
	@echo "  CC      $<"
	@$(CC) -c $< -o $@

$(MODULE):
	@echo "Compiling module..."
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

mount:
	mkdir mnt
	sudo insmod $(MODULE).ko
	sudo mount -t s2fs nodev mnt

unmount:
	sudo umount ./mnt 
	sudo rmmod $(MODULE).ko

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -rf mnt
