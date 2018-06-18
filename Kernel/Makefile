TARGET := $(LIBS)/andyos.bin

SRC_DIR := .
OBJ_DIR := obj

SRC_FILES := $(wildcard $(SRC_DIR)/**/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

ASM_FILES := $(wildcard $(SRC_DIR)/**/*.asm)
OBJ_FILES += $(patsubst $(SRC_DIR)/%.asm,$(OBJ_DIR)/%_.o,$(ASM_FILES))

LDFLAGS += $(LIBS)/libc.a -T link.ld

$(TARGET): $(OBJ_FILES) $(LIBS)/libc.a link.ld
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%_.o: $(SRC_DIR)/%.asm
	nasm -f elf32 -o $@ $<

clean:
	rm -f $(TARGET) $(OBJ_DIR)/**/*.o
	find . -type d -empty -delete