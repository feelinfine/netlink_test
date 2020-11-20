PWD := $(shell pwd)
BUILD_DIR := $(PWD)/build/
export BUILD_DIR

MKD := mkdir -p $(BUILD_DIR)

all: kernel user

kernel:
	$(MKD)
	$(MAKE) -C ./kernel

user:
	$(MKD) 
	$(MAKE) -C ./user

clean:
	$(MAKE) -C ./kernel clean
	$(MAKE) -C ./user clean

.PHONY: kernel user clean