TARGET = $(LIBS)/test

SRC_DIR := .
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

all: create_dirs $(TARGET)

create_dirs:
	mkdir -p obj

$(TARGET): $(OBJ_FILES) $(LIBS)/andyos.a $(LIBS)/libc.a
	$(CC) $(APP_LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	$(RM) -f $(TARGET) $(OBJS)