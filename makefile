# Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Executable name
TARGET = my_program

# Source files
SRCS = main.c assembler.c preprocessor.c lexer.c function.c tree.c vector.c

# Object files (replace .c with .o)
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Rule to compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(TARGET) $(OBJS)

# Phony targets
.PHONY: all clean
