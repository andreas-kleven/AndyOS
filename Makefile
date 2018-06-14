MAKE_DIR = $(CURDIR)
CC := /mnt/c/Tools/cross/bin/i686-elf-gcc
AR := ar

#ISO
ISO_DIR := $(MAKE_DIR)/Root
BOOT_DIR := $(ISO_DIR)/boot
ISO_NAME :=  $(MAKE_DIR)/AndyOS.iso

LIBS := $(MAKE_DIR)/Build

INC_SRCH_PATH := -I. 
INC_SRCH_PATH += -I$(MAKE_DIR)/Kernel/Include
INC_SRCH_PATH += -I$(MAKE_DIR)/Lib/Include
INC_SRCH_PATH += -I$(MAKE_DIR)/AndyOS/Include

LIB_SRCH_PATH := -L$(LIBS)

CPPFLAGS := $(INC_SRCH_PATH) $(LIB_SRCH_PATH) -w
CPPFLAGS += -g -ffreestanding -fno-exceptions -nostdlib -fno-rtti -fno-exceptions -lgcc

LDFLAGS := $(INC_SRCH_PATH) $(LIB_SRCH_PATH)
LDFLAGS += -g -ffreestanding -fno-exceptions -nostdlib -fno-rtti -fno-exceptions -lgcc

export MAKE_DIR CC CPPFLAGS LDFLAGS LIBS INC_SRCH_PATH

create_dir:
	mkdir -p $(LIBS)

libs: create_dir
	$(MAKE) -C Lib
	$(MAKE) -C AndyOS

apps: libs
	$(MAKE) -C Apps/WindowManager

kernel: libs
	$(MAKE) -C Kernel

all: libs kernel apps

iso: all
	cp $(LIBS)/andyos.bin $(BOOT_DIR)/andyos.bin
	grub-mkrescue -o $(ISO_NAME) $(ISO_DIR)

.PHONY: clean
clean:
	$(MAKE) -C Kernel clean
	$(MAKE) -C Lib clean
	$(MAKE) -C AndyOS clean
	$(MAKE) -C Apps/WindowManager clean
	rm -f $(BOOT_DIR)/andyos.bin