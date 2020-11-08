ARCH := x86
SYSROOT = $(HOME)/andyos
MAKE_DIR = $(CURDIR)
BUILD_DIR = $(MAKE_DIR)/Build
PROGRAMS_DIR = $(MAKE_DIR)/Programs

ISO_DIR = $(MAKE_DIR)/Root
BOOT_DIR = $(ISO_DIR)/boot
PROG_DIR = $(ISO_DIR)/usr/bin
ISO_NAME =  $(MAKE_DIR)/AndyOS.iso

ifeq ($(ARCH),x86)
    PREFIX = i686-elf-
else ifeq ($(ARCH),aarch64)
    PREFIX = aarch64-elf-
endif

CC := $(PREFIX)gcc
AR := $(PREFIX)ar
LD := $(PREFIX)ld
OBJCOPY := $(PREFIX)objcopy

export ARCH SYSROOT CC AR LD OBJCOPY MAKE_DIR PROGRAMS_DIR BUILD_DIR

create_dir:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BOOT_DIR)
	mkdir -p $(PROG_DIR)

programs:
	$(MAKE) -C $(PROGRAMS_DIR) all

kernel: create_dir
	$(MAKE) -C Kernel

all: kernel programs

iso: all
	cp $(BUILD_DIR)/kernel $(BOOT_DIR)/kernel
	cp $(BUILD_DIR)/winman $(PROG_DIR)/winman
	cp $(BUILD_DIR)/terminal $(PROG_DIR)/term
	cp $(BUILD_DIR)/test $(PROG_DIR)/test
	cp $(BUILD_DIR)/game $(PROG_DIR)/game
	cp $(BUILD_DIR)/mandelbrot $(PROG_DIR)/mndlbrt
	cp $(BUILD_DIR)/info $(PROG_DIR)/info
	cp $(BUILD_DIR)/audio $(PROG_DIR)/audio
	cp $(BUILD_DIR)/video $(PROG_DIR)/video
	grub-mkrescue -o $(ISO_NAME) $(ISO_DIR)

iso-kernel: kernel
	@if [ $( cmp -s $(BUILD_DIR)/kernel $(BOOT_DIR)/kernel) ]; then \
		cp $(BUILD_DIR)/kernel $(BOOT_DIR)/kernel; \
		grub-mkrescue -o $(ISO_NAME) $(ISO_DIR); \
	fi

.PHONY: clean
clean:
	$(MAKE) -C Kernel clean
	$(MAKE) -C $(PROGRAMS_DIR) clean
	rm -f $(BOOT_DIR)/kernel
	rm -f $(BUILD_DIR)/*
