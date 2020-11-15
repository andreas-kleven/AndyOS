TARGET = $(BUILD_DIR)/engine.a

INC_SRCH_PATH := -IInclude

SRC_DIR := .
OBJ_DIR := obj
SRC_FILES := $(shell find . -name '*.cpp')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

$(TARGET): $(BUILD_DIR)/gui.a $(BUILD_DIR)/gl.a $(OBJ_FILES)
	$(AR) cr $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(INC_SRCH_PATH) -Ofast -c -o $@ $<

.PHONY: clean
clean:
	$(RM) -f $(TARGET) $(OBJ_FILES)
