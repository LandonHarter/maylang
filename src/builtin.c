#include "types.h"
#include <stdio.h>

struct MayValue* builtin_print(struct MayValue** args, int arg_count) {
    printf("Hello world!\n");
    return NULL;
}
