#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <fcntl.h>

int open(const char *pathname, int flags, ...) {
    static int (*real_open)(const char *, int, ...) = NULL;

    if (!real_open) {
        real_open = dlsym(RTLD_NEXT, "open");
    }

    printf("[LD_PRELOAD LOG] open called: %s\n", pathname);

    // Handle variable argument for open()
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode_t mode = va_arg(args, mode_t);
        va_end(args);
        return real_open(pathname, flags, mode);
    }

    return real_open(pathname, flags);
}