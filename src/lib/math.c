#include "lib/math.h"
#include "types.h"
#include "env.h"
#include "error.h"
#include "module.h"
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>

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

struct MayValue* math_round(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = roundf(num->as.floating);
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

struct MayValue* math_sinh(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = sinhf(num->as.floating);
    return ret;
}

struct MayValue* math_cosh(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = coshf(num->as.floating);
    return ret;
}

struct MayValue* math_tanh(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = tanhf(num->as.floating);
    return ret;
}

struct MayValue* math_asinh(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = asinhf(num->as.floating);
    return ret;
}

struct MayValue* math_acosh(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = acoshf(num->as.floating);
    return ret;
}

struct MayValue* math_atanh(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = atanhf(num->as.floating);
    return ret;
}

struct MayValue* math_nlog(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = logf(num->as.floating);
    return ret;
}

struct MayValue* math_log2(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = log2f(num->as.floating);
    return ret;
}

struct MayValue* math_log(struct MayValue** args, int arg_count) {
    struct MayValue* num = args[0];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = log10f(num->as.floating);
    return ret;
}

struct MayValue* math_min(struct MayValue** args, int arg_count) {
    struct MayValue* num1 = args[0];
    struct MayValue* num2 = args[1];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = num1->as.floating > num2->as.floating ? num2->as.floating : num1->as.floating;
    return ret;
}

struct MayValue* math_max(struct MayValue** args, int arg_count) {
    struct MayValue* num1 = args[0];
    struct MayValue* num2 = args[1];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = num1->as.floating > num2->as.floating ? num1->as.floating : num2->as.floating;
    return ret;
}

struct MayValue* math_random(struct MayValue** args, int arg_count) {
    struct MayValue* min = args[0];
    struct MayValue* max = args[1];
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec ^ tv.tv_sec);

    float scale = (float)rand() / (float)RAND_MAX;
    ret->as.floating = min->as.floating + (max->as.floating - min->as.floating) * scale;

    return ret;
}

void load_math_env(struct Env* env) {
    struct MayValue* pi = malloc(sizeof(struct MayValue));
    pi->type = MAY_FLOAT;
    pi->as.floating = 3.14159265;
    register_builtin_var(env, "PI", pi);

    enum MayValueType* one_float = malloc(sizeof(enum MayValueType) * 1);
    one_float[0] = MAY_FLOAT;

    enum MayValueType* two_float = malloc(sizeof(enum MayValueType) * 1);
    two_float[0] = MAY_FLOAT;
    two_float[1] = MAY_FLOAT;

    register_builtin(env, "abs", math_abs, 1, one_float);
    register_builtin(env, "sqrt", math_sqrt, 1, one_float);
    register_builtin(env, "floor", math_floor, 1, one_float);
    register_builtin(env, "ceil", math_ceil, 1, one_float);
    register_builtin(env, "round", math_round, 1, one_float);

    register_builtin(env, "sin", math_sin, 1, one_float);
    register_builtin(env, "cos", math_cos, 1, one_float);
    register_builtin(env, "tan", math_tan, 1, one_float);
    register_builtin(env, "asin", math_asin, 1, one_float);
    register_builtin(env, "acos", math_acos, 1, one_float);
    register_builtin(env, "atan", math_atan, 1, one_float);
    register_builtin(env, "sinh", math_sinh, 1, one_float);
    register_builtin(env, "cosh", math_cosh, 1, one_float);
    register_builtin(env, "tanh", math_tanh, 1, one_float);
    register_builtin(env, "asinh", math_asinh, 1, one_float);
    register_builtin(env, "acosh", math_acosh, 1, one_float);
    register_builtin(env, "atanh", math_atanh, 1, one_float);

    register_builtin(env, "nlog", math_nlog, 1, one_float);
    register_builtin(env, "log2n", math_log2, 1, one_float);
    register_builtin(env, "log", math_log, 1, one_float);

    register_builtin(env, "pow", math_pow, 2, two_float);
    register_builtin(env, "min", math_min, 2, two_float);
    register_builtin(env, "max", math_max, 2, two_float);
    register_builtin(env, "random", math_random, 2, two_float);
}

struct Module* load_math_module() {
    struct Module* mod = malloc(sizeof(struct Module));
    mod->name = "math";
    mod->loader = load_math_env;
    return mod;
}
