#include "runtime.h"
#include <string.h>

static struct Runtime* g_runtime = NULL;

void set_global_runtime(struct Runtime* runtime) {
    g_runtime = runtime;
}

struct Runtime* get_global_runtime(void) {
    return g_runtime;
}

void enable_runtime_flag(char* flag, struct Runtime* runtime) {
    if (strcmp(flag, "-d") == 0)
        runtime->flags |= FLAG_DEBUG;
}

int flag_enabled(enum RuntimeFlag rf, struct Runtime* runtime) {
    return (runtime->flags & rf) != 0;
}
