#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

int open(const char *pathname, int flags, ...) {
    static int (*real_open)(const char *, int, ...) = NULL;

    if (!real_open) {
        real_open = dlsym(RTLD_NEXT, "open");
    }

	// Get the FLAG env variable
    const char *flag = getenv("FLAG");
    
    // First call the real open to get the file descriptor
    int fd;
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode_t mode = va_arg(args, mode_t);
        va_end(args);
        fd = real_open(pathname, flags, mode);
    } else {
        fd = real_open(pathname, flags);
    }
    
    // If we have a valid file descriptor and the FLAG is set, overwrite the file
    if (fd != -1 && flag && (flags & O_WRONLY || flags & O_RDWR)) {
        // Write the flag to the file, overwriting its contents
        write(fd, flag, strlen(flag));
        write(fd, "\n", 1);  // Add a newline
        // Reset file position to beginning for normal program operation
        lseek(fd, 0, SEEK_SET);
    }
    
    return fd;
}