CC = clang

SRC_DIR = src
OBJ_DIR = obj
OUT_DIR = bin

# CFLAGS = -D_CRT_SECURE_NO_WARNINGS
CFLAGS = -DEMU_TESTS_ENABLED

LDFLAGS = -lm

SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

TARGET = $(OUT_DIR)/out

all: $(TARGET)
dev: $(TARGET) run

$(TARGET): $(OBJS) $(OUT_DIR)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
# 	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
# 	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"
	@mkdir -p $@

$(OUT_DIR):
# 	@if not exist "$(OUT_DIR)" mkdir "$(OUT_DIR)"
	@mkdir -p $@
clean:
# 	@if exist "$(OBJ_DIR)" rmdir /s /q "$(OBJ_DIR)"
	@rm -rf $(OBJ_DIR) $(OUT_DIR)

run: $(TARGET)
	./$(TARGET) --test tests/isa/rv64ui-p-ma_data

.PHONY: all dev clean run
