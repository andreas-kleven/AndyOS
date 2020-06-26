PREFIX = i686-andyos-
CC := $(PREFIX)gcc
AR := $(PREFIX)ar
LD := $(PREFIX)ld
OBJCOPY := $(PREFIX)objcopy

export CC AR LD OBJCOPY

INC_SRCH_PATH := -isystem$(SYSROOT)/usr/include
INC_SRCH_PATH += -I$(PROGRAMS_DIR)/AndyOS/Include
INC_SRCH_PATH += -I$(SYSROOT)/usr/include/c++/8.1.0
INC_SRCH_PATH += -I.
INC_SRCH_PATH += -I$(MAKE_DIR)/Kernel/Include
INC_SRCH_PATH += -I$(PROGRAMS_DIR)
INC_SRCH_PATH += -I$(PROGRAMS_DIR)/Include

LIB_SRCH_PATH := -L$(BUILD_DIR) -L$(SYSROOT)/usr/lib

LIB_FILES := $(BUILD_DIR)/andyos.a $(SYSROOT)/usr/lib/crt0.o $(SYSROOT)/usr/lib/crtbegin.o $(SYSROOT)/usr/lib/crtend.o $(SYSROOT)/usr/lib/crti.o $(SYSROOT)/usr/lib/crtn.o -lstdc++ -lsupc++ -lgcc -lc -lg -lm

CPPFLAGS := $(INC_SRCH_PATH) $(LIB_SRCH_PATH)
CPPFLAGS += -g -O0
CPPFLAGS += -Wno-sign-compare -Wno-unused-but-set-variable -Wno-unused-variable -Wall
CPPFLAGS += -fno-omit-frame-pointer

LDFLAGS := $(LIB_SRCH_PATH)
LDFLAGS += $(LIB_FILES)
LDFLAGS += --allow-multiple-definition # __errno

export CPPFLAGS LDFLAGS INC_SRCH_PATH PROGRAMS_DIR

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
	rm -f $(BUILD_DIR)/*