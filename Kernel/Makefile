TARGET := $(BUILD_DIR)/andyos.bin

SRC_DIR := .
OBJ_DIR := obj

SRC_FILES := $(wildcard $(SRC_DIR)/**/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

ASM_FILES := $(wildcard $(SRC_DIR)/**/*.asm)
OBJ_FILES += $(patsubst $(SRC_DIR)/%.asm,$(OBJ_DIR)/%_.o,$(ASM_FILES))

INC_SRCH_PATH := -I.
INC_SRCH_PATH += -I$(MAKE_DIR)/Kernel/Include
INC_SRCH_PATH += -I$(MAKE_DIR)/Kernel/Lib

CPPFLAGS := $(INC_SRCH_PATH) -w
CPPFLAGS += -g -ffreestanding -fno-exceptions -nostdlib -fno-rtti -fno-exceptions

LDFLAGS := $(INC_SRCH_PATH)
LDFLAGS += -g -ffreestanding -fno-exceptions -nostdlib -fno-rtti -fno-exceptions
LDFLAGS += -T link.ld

$(TARGET): $(OBJ_FILES) link.ld
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%_.o: $(SRC_DIR)/%.asm
	nasm -f elf32 -o $@ $<

clean:
	rm -f $(TARGET) $(OBJ_DIR)/**/*.o
	find . -type d -empty -delete