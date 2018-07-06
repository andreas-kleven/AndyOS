MAKE_DIR = $(CURDIR)
CC := i686-elf-gcc
AR := ar

BUILD_DIR := $(MAKE_DIR)/Build
PROGRAMS_DIR = $(MAKE_DIR)/Programs

export CC AR MAKE_DIR PROGRAMS_DIR BUILD_DIR

ISO_DIR := $(MAKE_DIR)/Root
BOOT_DIR := $(ISO_DIR)/boot
ISO_NAME :=  $(MAKE_DIR)/AndyOS.iso

create_dir:
	mkdir -p $(BUILD_DIR)

programs:
	$(MAKE) -C $(PROGRAMS_DIR) all

kernel: create_dir
	$(MAKE) -C Kernel

all: kernel programs

iso: all
	cp $(BUILD_DIR)/andyos.bin $(BOOT_DIR)/andyos.bin
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
	rm -f $(BOOT_DIR)/andyos.bin
	rm -f $(BUILD_DIR)/*