TARGET = $(BUILD_DIR)/game

INC_SRCH_PATH := -I$(PROGRAMS_DIR)/GameEngine/Include

SRC_DIR := .
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

CPPFLAGS += $(INC_SRCH_PATH)
LDFLAGS += $(INC_SRCH_PATH)

$(TARGET): $(OBJ_FILES) $(BUILD_DIR)/engine.a $(BUILD_DIR)/gl.a $(BUILD_DIR)/gui.a
	$(LD) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -Ofast -c -o $@ $<

.PHONY: clean
clean:
	$(RM) -f $(TARGET) $(OBJ_FILES)