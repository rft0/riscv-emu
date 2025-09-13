CC = clang

SRC_DIR = src
OBJ_DIR = obj
OUT_DIR = bin

CFLAGS =
LDFLAGS =

SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

TARGET = $(OUT_DIR)/out.exe

all: $(TARGET) run

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"

$(OUT_DIR):
	@if not exist "$(OUT_DIR)" mkdir "$(OUT_DIR)"

clean:
	@if exist "$(OBJ_DIR)" rmdir /s /q "$(OBJ_DIR)"

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run

