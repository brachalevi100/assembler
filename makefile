# Makefile - Windows (MinGW) / Linux

CC     = gcc
CFLAGS = -Wall -g -I include

TARGET = assembler.exe

SRC_DIR = src
OBJ_DIR = obj

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/assembler.c \
       $(SRC_DIR)/preprosesor.c \
       $(SRC_DIR)/lexer.c \
       $(SRC_DIR)/function.c \
       $(SRC_DIR)/tree.c \
       $(SRC_DIR)/vector.c

OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(OBJ_DIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

clean:
	del /f /q $(TARGET) 2>nul || true
	del /f /q $(OBJ_DIR)\*.o 2>nul || true
	rmdir /q $(OBJ_DIR) 2>nul || true

.PHONY: all clean