# Makefile - Windows (MinGW)

CC     = gcc
CFLAGS = -Wall -g

TARGET = assembler.exe

SRCS = main.c assembler.c preprosesor.c lexer.c function.c tree.c vector.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /f /q $(TARGET) $(OBJS) 2>nul || true

.PHONY: all clean