TARGET = $(BUILD_DIR)/engine.a

INC_SRCH_PATH := -IInclude

SRC_DIR := .
OBJ_DIR := obj
SRC_FILES := $(shell find . -name '*.cpp')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

$(TARGET): $(OBJ_FILES)
	$(AR) cr $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(INC_SRCH_PATH) -c -o $@ $<

.PHONY: clean
clean:
	$(RM) -f $(TARGET) $(OBJ_FILES)