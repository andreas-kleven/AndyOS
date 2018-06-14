TARGET := $(LIBS)/andyos.bin

SRC_DIR := .
OBJ_DIR := obj

SRC_FILES := $(wildcard $(SRC_DIR)/**/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

ASM_FILES := $(wildcard $(SRC_DIR)/**/*.asm)
OBJ_FILES += $(patsubst $(SRC_DIR)/%.asm,$(OBJ_DIR)/%_.o,$(ASM_FILES))

LDFLAGS += $(LIBS)/libc.a -T link.ld

all: create_dirs $(TARGET)

create_dirs:
	rsync -av --exclude "$(OBJ_DIR)" -f"+ */" -f"- *" "$(SRC_DIR)" "$(OBJ_DIR)"

$(TARGET): $(OBJ_FILES) link.ld $(LIBS)/libc.a
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(OBJ_DIR)/%_.o: $(SRC_DIR)/%.asm
	nasm -f elf32 -o $@ $<

clean:
	rm -f $(TARGET) $(OBJ_DIR)/**/*.o
	find . -type d -empty -delete