#include "debug.h"
#include "runtime.h"
#include <stdio.h>

void debug_printf(const char* c) {
    struct Runtime* rt = get_global_runtime();
    if (rt && flag_enabled(FLAG_DEBUG, rt)) {
        printf("[DEBUG] %s\n", c);
    }
}

void debug_error(const char* c) {
    struct Runtime* rt = get_global_runtime();
    if (rt && flag_enabled(FLAG_DEBUG, rt)) {
        fprintf(stderr, "[DEBUG] %s\n", c);
    }
}
