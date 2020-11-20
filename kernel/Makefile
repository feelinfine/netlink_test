
KERNELRELEASE ?= $(shell uname -r)
KERNELDIR ?= /lib/modules/$(KERNELRELEASE)/build
PWD := $(shell pwd)

obj-m += kernel_mod.o
kernel_mod-objs := kernel.o

ccflags-y += -I$(src)/../netlink 
ccflags-y += -DBUILD_DIR=$(BUILD_DIR)

all: module

module:
	@$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	mv kernel_mod.ko $(BUILD_DIR)/kernel_mod.ko 

clean:
	@$(MAKE) -C $(KERNELDIR) M=$(PWD) clean