ARCH := aarch64

MAKE_DIR = $(CURDIR)
BUILD_DIR = $(MAKE_DIR)/Build
PROGRAMS_DIR = $(MAKE_DIR)/Programs

ISO_DIR = $(MAKE_DIR)/Root
BOOT_DIR = $(ISO_DIR)/boot
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

export ARCH CC AR LD OBJCOPY MAKE_DIR PROGRAMS_DIR BUILD_DIR

create_dir:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BOOT_DIR)

programs:
	$(MAKE) -C $(PROGRAMS_DIR) all

kernel: create_dir
	$(MAKE) -C Kernel

all: kernel programs

iso: all
	cp $(BUILD_DIR)/kernel.bin $(BOOT_DIR)/kernel.bin
	cp $(BUILD_DIR)/winman $(ISO_DIR)/1winman
	cp $(BUILD_DIR)/terminal $(ISO_DIR)/1term
	cp $(BUILD_DIR)/test $(ISO_DIR)/1test
	cp $(BUILD_DIR)/game $(ISO_DIR)/1game
	cp $(BUILD_DIR)/mandelbrot $(ISO_DIR)/1mndlbrt
	grub-mkrescue -o $(ISO_NAME) $(ISO_DIR)

.PHONY: clean
clean:
	$(MAKE) -C Kernel clean
	$(MAKE) -C $(PROGRAMS_DIR) clean
	rm -f $(BOOT_DIR)/kernel.bin
	rm -f $(BUILD_DIR)/*