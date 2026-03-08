#include "types.h"
#include <stdio.h>

struct MayValue* builtin_print(struct MayValue** args, int arg_count) {
    printf("%s\n", args[0]->as.string.val);
    return NULL;
}
