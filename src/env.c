#include "env.h"
#include "types.h"
#include "module.h"
#include "lib/math.h"
#include "lib/io.h"
#include "lib/string.h"
#include "lib/default.h"
#include <stdlib.h>
#include <string.h>

struct Env* new_env(struct Env* parent) {
    struct Env* env = malloc(sizeof(struct Env));

    env->vars = malloc(sizeof(struct VarList));
    env->vars->vars = NULL;
    env->vars->count = 0;
    env->vars->capacity = 0;

    env->modules = malloc(sizeof(struct ModuleList));
    env->modules->modules = NULL;
    env->modules->count = 0;
    env->modules->capacity = 0;

    env->parent = parent;

    register_module(env->modules, load_math_module());
    register_module(env->modules, load_io_module());
    register_module(env->modules, load_string_module());
    load_default_module(env);

    return env;
}

void free_val(struct MayValue* val) {
    free(val);
}

void free_var(struct Var* var) {
    free(var->name);
    free_val(var->value);
}

void free_var_list(struct VarList* list) {
    free(list->vars);
    list->vars = NULL;
    list->count = 0;
    list->capacity = 0;
}

void free_env(struct Env* env) {
    if (env->vars) {
        free_var_list(env->vars);
    }
    if (env->modules) {
        free_module_list(env->modules);
    }
    free(env);
}

void append_var(struct Env* env, struct Var* var) {
    if (env->vars->count >= env->vars->capacity) {
        env->vars->capacity = env->vars->capacity == 0 ? 8 : env->vars->capacity * 2;
        env->vars->vars = realloc(env->vars->vars, env->vars->capacity * sizeof(struct Var*));
    }
    env->vars->vars[env->vars->count++] = var;
}

void register_builtin(struct Env* env, const char* name, MayNativeFn fn, int num_args, enum MayValueType* arg_types) {
    struct MayValue* value = malloc(sizeof(struct MayValue));
    value->type = MAY_BUILTIN_FUNC;
    value->as.builtin.name = strdup(name);
    value->as.builtin.cfunc = fn;
    value->as.builtin.num_args = num_args;
    value->as.builtin.arg_types = arg_types;

    struct Var* var = malloc(sizeof(struct Var));
    var->name = strdup(name);
    var->value = value;
    append_var(env, var);
}

void register_builtin_var(struct Env* env, const char* name, struct MayValue* val) {
    struct Var* var = malloc(sizeof(struct Var));
    var->name = strdup(name);
    var->value = val;
    append_var(env, var);
}
