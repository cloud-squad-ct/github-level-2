#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
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