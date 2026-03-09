#include "lib/math.h"
#include "types.h"
#include "env.h"
#include "error.h"
#include "module.h"
#include <math.h>
#include <stdlib.h>

struct MayValue* math_abs(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));

    ret->type = MAY_FLOAT;
    if (num->as.floating < 0) {
        ret->as.floating = -num->as.floating;
    } else {
        ret->as.floating = num->as.floating;
    }

    return ret;
}

struct MayValue* math_sqrt(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = sqrt((double)num->as.floating);
    return ret;
}

void load_math_env(struct Env* env) {
    enum MayValueType* atabs = malloc(sizeof(enum MayValueType) * 1);
    atabs[0] = MAY_FLOAT;
    register_builtin(env, "abs", math_abs, 1, atabs);

    enum MayValueType* atsqrt = malloc(sizeof(enum MayValueType) * 1);
    atsqrt[0] = MAY_FLOAT;
    register_builtin(env, "sqrt", math_sqrt, 1, atsqrt);
}

struct Module* load_math_module() {
    struct Module* mod = malloc(sizeof(struct Module));
    mod->name = "math";
    mod->loader = load_math_env;
    return mod;
}
