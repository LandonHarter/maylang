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
    ret->as.floating = sqrtf(num->as.floating);
    return ret;
}

struct MayValue* math_floor(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = floorf(num->as.floating);
    return ret;
}

struct MayValue* math_ceil(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = ceilf(num->as.floating);
    return ret;
}

struct MayValue* math_pow(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* power = args[1];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = powf(num->as.floating, power->as.floating);
    return ret;
}

struct MayValue* math_sin(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = sinf(num->as.floating);
    return ret;
}

struct MayValue* math_cos(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = cosf(num->as.floating);
    return ret;
}

struct MayValue* math_tan(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = tanf(num->as.floating);
    return ret;
}

struct MayValue* math_asin(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = asinf(num->as.floating);
    return ret;
}

struct MayValue* math_acos(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = acosf(num->as.floating);
    return ret;
}

struct MayValue* math_atan(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = atanf(num->as.floating);
    return ret;
}

void load_math_env(struct Env* env) {
    struct MayValue* pi = malloc(sizeof(struct MayValue));
    pi->type = MAY_FLOAT;
    pi->as.floating = 3.14159265;
    register_builtin_var(env, "PI", pi);

    enum MayValueType* one_float = malloc(sizeof(enum MayValueType) * 1);
    one_float[0] = MAY_FLOAT;

    register_builtin(env, "abs", math_abs, 1, one_float);
    register_builtin(env, "sqrt", math_sqrt, 1, one_float);
    register_builtin(env, "floor", math_floor, 1, one_float);
    register_builtin(env, "ceil", math_ceil, 1, one_float);

    register_builtin(env, "sin", math_sin, 1, one_float);
    register_builtin(env, "cos", math_cos, 1, one_float);
    register_builtin(env, "tan", math_tan, 1, one_float);
    register_builtin(env, "asin", math_asin, 1, one_float);
    register_builtin(env, "acos", math_acos, 1, one_float);
    register_builtin(env, "atan", math_atan, 1, one_float);

    enum MayValueType* two_float = malloc(sizeof(enum MayValueType) * 1);
    two_float[0] = MAY_FLOAT;
    two_float[1] = MAY_FLOAT;
    register_builtin(env, "pow", math_pow, 2, two_float);
}

struct Module* load_math_module() {
    struct Module* mod = malloc(sizeof(struct Module));
    mod->name = "math";
    mod->loader = load_math_env;
    return mod;
}
