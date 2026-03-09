#include "lib/math.h"
#include "types.h"
#include "env.h"
#include "error.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MayValue* default_to_string(struct MayValue** args, int arg_count) {
    struct MayValue* result = malloc(sizeof(struct MayValue));
    result->type = MAY_STRING;
    result->as.string.val = mv_to_string(args[0]);
    result->as.string.len = strlen(result->as.string.val);
    return result;
}

void load_default_module(struct Env* env) {
    enum MayValueType* attostring = malloc(sizeof(enum MayValueType) * 1);
    attostring[0] = MAY_VAR;
    register_builtin(env, "toString", default_to_string, 1, attostring);
}
