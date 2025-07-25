# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Targets
TARGET = prog

# Default rule
all: $(TARGET)

# Rule to build the binary
$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ main.c

# Clean everything
clean:
	rm -f $(TARGET)
