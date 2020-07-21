TARGET = $(BUILD_DIR)/mandelbrot

SRC_DIR := .
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

$(TARGET): $(OBJ_FILES) $(BUILD_DIR)/gui.a $(BUILD_DIR)/gl.a $(BUILD_DIR)/andyos.a
	$(LD) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -Ofast -c -o $@ $<

.PHONY: clean
clean:
	$(RM) -f $(TARGET) $(OBJ_FILES)