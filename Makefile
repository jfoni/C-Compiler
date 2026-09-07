CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

BUILD_DIR = build
TARGET = $(BUILD_DIR)/mycompiler.exe
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)

.PHONY: all clean