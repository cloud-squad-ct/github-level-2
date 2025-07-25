# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Target name
TARGET = prog

# Build rule
all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

# Clean rule
clean:
	rm -f $(TARGET)