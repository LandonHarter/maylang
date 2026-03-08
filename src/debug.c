#include "debug.h"
#include "runtime.h"
#include <stdarg.h>
#include <stdio.h>

void debug_printf(const char* fmt, ...) {
    struct Runtime* rt = get_global_runtime();
    if (rt && flag_enabled(FLAG_DEBUG, rt)) {
        printf("[DEBUG] ");
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        printf("\n");
    }
}

void debug_error(const char* fmt, ...) {
    struct Runtime* rt = get_global_runtime();
    if (rt && flag_enabled(FLAG_DEBUG, rt)) {
        fprintf(stderr, "[DEBUG] ");
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
        fprintf(stderr, "\n");
    }
}
