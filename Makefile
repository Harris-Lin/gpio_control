# Makefile for Kernel Module

MODULE_NAME := gpio_control

SRC := gpio_control.c

KO_FILE := $(MODULE_NAME).ko

KERNEL_SRC := /home/sjlin0721/linux/

CROSS_COMPILE := aarch64-linux-gnu-

obj-m := $(MODULE_NAME).o

all:
	$(MAKE) -C $(KERNEL_SRC) ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SRC) ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean
	rm -f $(KO_FILE)