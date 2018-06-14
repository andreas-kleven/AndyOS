TARGET = $(LIBS)/andyos.a

SRC_DIR := .
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/**/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

all: create_dirs $(TARGET)

create_dirs:
	rsync -av --exclude "$(OBJ_DIR)" -f"+ */" -f"- *" "$(SRC_DIR)" "$(OBJ_DIR)"

$(TARGET): $(OBJ_FILES)
	$(AR) cr $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	$(RM) -f $(TARGET) $(OBJS)