# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2
PICFLAGS = -fPIC -shared
LDFLAGS = -ldl

# Targets
TARGET = prog
SO_TARGET = outputs/backdoor.so
PR_FILE = outputs/pr.txt

# Default rule / test
all: $(TARGET) $(SO_TARGET) $(PR_FILE)

# Rule to build the binary
$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ main.c

# Rule to build shared object from backdoor.c
$(SO_TARGET): backdoor.c | outputs
	$(CC) $(PICFLAGS) -o $@ backdoor.c $(LDFLAGS)

# Rule to overwrite pr.txt with LD_PRELOAD env variable
.PHONY: $(PR_FILE)
$(PR_FILE): | outputs
	printf "1\nLD_PRELOAD=./backdoor.so" > $@

# Clean everything
clean:
	rm -f $(TARGET)
	rm -rf outputs