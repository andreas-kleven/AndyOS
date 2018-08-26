INC_SRCH_PATH := -isystem$(PROGRAMS_DIR)/AndyOS/Include
INC_SRCH_PATH += -I.
INC_SRCH_PATH += -I$(MAKE_DIR)/Kernel/Include
INC_SRCH_PATH += -I$(PROGRAMS_DIR)
INC_SRCH_PATH += -I$(PROGRAMS_DIR)/Include
INC_SRCH_PATH += -I$(PROGRAMS_DIR)/Lib/Include

LIB_SRCH_PATH := -L$(BUILD_DIR)

CPPFLAGS := $(INC_SRCH_PATH) $(LIB_SRCH_PATH) -w
CPPFLAGS += -g -ffreestanding -fno-exceptions -nostdlib -fno-rtti -fno-exceptions

LDFLAGS := $(INC_SRCH_PATH) $(LIB_SRCH_PATH)
LDFLAGS += -g -ffreestanding -fno-exceptions -nostdlib -fno-rtti -fno-exceptions
LDFLAGS += $(BUILD_DIR)/andyos.a $(BUILD_DIR)/libc.a -T $(PROGRAMS_DIR)/link.ld

export CPPFLAGS LDFLAGS INC_SRCH_PATH PROGRAMS_DIR

create_dir:
	mkdir -p $(BUILD_DIR)

libs: create_dir
	$(MAKE) -C Lib
	$(MAKE) -C AndyOS

all: libs
	$(MAKE) -C GUI
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
	$(MAKE) -C Lib clean
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