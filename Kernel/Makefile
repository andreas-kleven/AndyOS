MAKE_DIR = $(CURDIR)
TARGET := $(BUILD_DIR)/kernel

SRC_DIR := .
OBJ_DIR := obj
ARCH_DIR := Arch/$(ARCH)

SRC_FILES := $(shell find $(SRC_DIR)/ -name '*.cpp' -not -path "$(SRC_DIR)/Arch/*")
SRC_FILES += $(wildcard $(ARCH_DIR)/**/*.cpp)
OBJ_FILES := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
			 
ASM_FILES := $(shell find $(SRC_DIR)/ -name '*.S' -not -path "$(SRC_DIR)/Arch/*")
ASM_FILES += $(wildcard $(ARCH_DIR)/**/*.S)
OBJ_FILES += $(patsubst %.S,$(OBJ_DIR)/%_.o,$(ASM_FILES))

ifeq ($(ARCH),x86)
ASM_FILES2 := $(shell find $(SRC_DIR)/ -name '*.asm' -not -path "$(SRC_DIR)/Arch/*")
ASM_FILES2 += $(wildcard $(ARCH_DIR)/**/*.asm)
OBJ_FILES += $(patsubst %.asm,$(OBJ_DIR)/%_.o,$(ASM_FILES2))
endif

INC_SRCH_PATH := -IInclude
INC_SRCH_PATH += -ILib
INC_SRCH_PATH += -I$(ARCH_DIR)/Include

BASEFLAGS := $(INC_SRCH_PATH) -nostdinc -nostdlib -nostartfiles
LINKSCRIPT := $(ARCH_DIR)/link.ld

CPPFLAGS := $(BASEFLAGS)
CPPFLAGS += -Wno-shift-count-overflow -Wno-unused-variable -Wall -g -fno-rtti -fno-exceptions -fno-stack-protector -ffreestanding

LDFLAGS := $(BASEFLAGS) -T $(LINKSCRIPT)

CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)

$(TARGET): $(CRTBEGIN_OBJ) $(OBJ_FILES) $(CRTEND_OBJ)
ifeq ($(ARCH),x86)
	$(CC) $(LDFLAGS) -o $@ $^
else ifeq ($(ARCH),aarch64)
	$(LD) $(LDFLAGS) -o $@ $^
	$(OBJCOPY) -O binary $@ $(BUILD_DIR)/kernel8.img
endif

-include $(OBJ_FILES:.o=.d)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) -MP -MD $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%_.o: $(SRC_DIR)/%.S
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%_.o: $(SRC_DIR)/%.asm
	nasm -f elf32 -o $@ $<

clean:
	rm -f $(TARGET) $(OBJ_DIR)/**/*.o $(OBJ_DIR)/**/*.d
	find . -type d -empty -delete