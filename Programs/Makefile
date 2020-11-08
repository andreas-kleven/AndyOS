PREFIX = i686-andyos-
CC := $(PREFIX)gcc
AR := $(PREFIX)ar
LD := $(PREFIX)ld
OBJCOPY := $(PREFIX)objcopy

export CC AR LD OBJCOPY

INC_SRCH_PATH := -I.
INC_SRCH_PATH += -I$(MAKE_DIR)/Kernel/Include
INC_SRCH_PATH += -I$(PROGRAMS_DIR)/AndyOS/Include
INC_SRCH_PATH += -I$(PROGRAMS_DIR)
INC_SRCH_PATH += -I$(PROGRAMS_DIR)/Include

CPPFLAGS := $(INC_SRCH_PATH)
CPPFLAGS += -g
CPPFLAGS += -Wno-sign-compare -Wno-unused-but-set-variable -Wno-unused-variable -Wall

LDFLAGS := -Wl,--allow-multiple-definition # __errno

LIBS := $(BUILD_DIR)/andyos.a

export CPPFLAGS LDFLAGS LIBS

create_dir:
	mkdir -p $(BUILD_DIR)

libs: create_dir
	$(MAKE) -C AndyOS
	$(MAKE) -C GUI

all: libs
	$(MAKE) -C WindowManager
	$(MAKE) -C Terminal
	$(MAKE) -C Test
	$(MAKE) -C GL
	$(MAKE) -C GameEngine
	$(MAKE) -C 3DGame
	$(MAKE) -C Mandelbrot
	$(MAKE) -C Info
	$(MAKE) -C Audio
	$(MAKE) -C Video

.PHONY: clean
clean:
	$(MAKE) -C AndyOS clean
	$(MAKE) -C GUI clean
	$(MAKE) -C WindowManager clean
	$(MAKE) -C Terminal clean
	$(MAKE) -C Test clean
	$(MAKE) -C GL clean
	$(MAKE) -C GameEngine clean
	$(MAKE) -C 3DGame clean
	$(MAKE) -C Mandelbrot clean
	$(MAKE) -C Info clean
	$(MAKE) -C Audio clean
	$(MAKE) -C Video clean
	rm -f $(BUILD_DIR)/*
