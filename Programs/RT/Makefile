SRC_DIR := .
OBJ_DIR := $(BUILD_DIR)
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) -c -o $@ $<

all: $(OBJ_FILES)

.PHONY: clean
clean:
	$(RM) -f $(TARGET) $(OBJ_FILES)