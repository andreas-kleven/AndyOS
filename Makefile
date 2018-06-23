MAKE_DIR = $(CURDIR)
CC := i686-elf-gcc
AR := ar

#ISO
ISO_DIR := $(MAKE_DIR)/Root
BOOT_DIR := $(ISO_DIR)/boot
ISO_NAME :=  $(MAKE_DIR)/AndyOS.iso

LIBS := $(MAKE_DIR)/Build

INC_SRCH_PATH := -isystem$(MAKE_DIR)/AndyOS/Include
INC_SRCH_PATH += -I.
INC_SRCH_PATH += -I$(MAKE_DIR)/Kernel/Include
INC_SRCH_PATH += -I$(MAKE_DIR)/Lib/Include
INC_SRCH_PATH += -I$(MAKE_DIR)/Apps
INC_SRCH_PATH += -I$(MAKE_DIR)/Apps/Include

LIB_SRCH_PATH := -L$(LIBS)

CPPFLAGS := $(INC_SRCH_PATH) $(LIB_SRCH_PATH) -w
CPPFLAGS += -g -ffreestanding -fno-exceptions -nostdlib -fno-rtti -fno-exceptions

LDFLAGS := $(INC_SRCH_PATH) $(LIB_SRCH_PATH)
LDFLAGS += -g -ffreestanding -fno-exceptions -nostdlib -fno-rtti -fno-exceptions

APP_LDFLAGS := $(LDFLAGS) $(LIBS)/andyos.a $(LIBS)/libc.a -T $(MAKE_DIR)/Apps/link.ld

export MAKE_DIR CC CPPFLAGS LDFLAGS LIBS INC_SRCH_PATH
export APP_LDFLAGS

create_dir:
	mkdir -p $(LIBS)

libs: create_dir
	$(MAKE) -C Lib
	$(MAKE) -C AndyOS

apps: libs
	$(MAKE) -C Apps/WindowManager
	$(MAKE) -C Apps/Test
	$(MAKE) -C Apps/GL
	$(MAKE) -C Apps/GameEngine
	$(MAKE) -C Apps/3DGame

kernel: libs
	$(MAKE) -C Kernel

all: libs kernel apps

iso: all
	cp $(LIBS)/andyos.bin $(BOOT_DIR)/andyos.bin
	cp $(LIBS)/winman $(ISO_DIR)/1winman
	cp $(LIBS)/test $(ISO_DIR)/1test
	cp $(LIBS)/game $(ISO_DIR)/1game
	grub-mkrescue -o $(ISO_NAME) $(ISO_DIR)

.PHONY: clean
clean:
	$(MAKE) -C Kernel clean
	$(MAKE) -C Lib clean
	$(MAKE) -C AndyOS clean
	$(MAKE) -C Apps/WindowManager clean
	$(MAKE) -C Apps/Test clean
	$(MAKE) -C Apps/GL clean
	$(MAKE) -C Apps/GameEngine clean
	$(MAKE) -C Apps/3DGame clean
	rm -f $(BOOT_DIR)/andyos.bin
	rm -f $(LIBS)/*