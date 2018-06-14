MAKE_DIR = $(CURDIR)
CC := /mnt/c/Tools/cross/bin/i686-elf-gcc
AR := ar

#ISO
ISO_DIR := $(MAKE_DIR)/Root
BOOT_DIR := $(ISO_DIR)/boot
ISO_NAME :=  $(MAKE_DIR)/AndyOS.iso

KERNEL_DIR	:= $(MAKE_DIR)/Kernel
LIB_DIR		:= $(MAKE_DIR)/Lib

LIBS := $(MAKE_DIR)/Build

INC_SRCH_PATH := -I. -I$(KERNEL_DIR)/Include
INC_SRCH_PATH += -I$(LIB_DIR)/Include

LIB_SRCH_PATH := -L$(LIBS)

CPPFLAGS := $(INC_SRCH_PATH) $(LIB_SRCH_PATH) -w
CPPFLAGS += -g -ffreestanding -fno-exceptions -nostdlib -fno-rtti -fno-exceptions -lgcc

LDFLAGS := $(INC_SRCH_PATH) $(LIB_SRCH_PATH)
LDFLAGS += -g -ffreestanding -fno-exceptions -nostdlib -fno-rtti -fno-exceptions -lgcc

export MAKE_DIR CC CPPFLAGS LDFLAGS LIBS INC_SRCH_PATH

all:
	mkdir -p $(LIBS)
	$(MAKE) -C Lib
	$(MAKE) -C Kernel

iso: all
	cp $(LIBS)/andyos.bin $(BOOT_DIR)/andyos.bin
	grub-mkrescue -o $(ISO_NAME) $(ISO_DIR)


.PHONY: clean
clean:
	$(MAKE) -C Lib clean
	$(MAKE) -C Kernel clean
	rm -f $(BOOT_DIR)/andyos.bin