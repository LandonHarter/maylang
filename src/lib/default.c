#include "lib/math.h"
#include "types.h"
#include "env.h"
#include "error.h"
#include "module.h"
#include <stdlib.h>
#include <string.h>

struct MayValue* default_to_string(struct MayValue** args, int arg_count) {
    struct MayValue* result = malloc(sizeof(struct MayValue));
    result->type = MAY_STRING;
    result->as.string.val = mv_to_string(args[0]);
    result->as.string.len = strlen(result->as.string.val);
    return result;
}

struct MayValue* default_length(struct MayValue** args, int arg_count) {
    struct MayValue* result = malloc(sizeof(struct MayValue));
    result->type = MAY_INT;

    enum MayValueType type = args[0]->type;
    if (type == MAY_VAR) {
        type = args[0]->inferred;
    }

    switch (type) {
        case MAY_STRING:
            result->as.integer = strlen(args[0]->as.string.val);
            break;
        default:
            result->as.integer = 0;
    }

    return result;
}

void load_default_module(struct Env* env) {
    enum MayValueType* attostring = malloc(sizeof(enum MayValueType) * 1);
    attostring[0] = MAY_VAR;
    register_builtin(env, "toString", default_to_string, 1, attostring);

    enum MayValueType* atlength = malloc(sizeof(enum MayValueType) * 1);
    atlength[0] = MAY_VAR;
    register_builtin(env, "length", default_length, 1, atlength);
}
