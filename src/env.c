#include "env.h"
#include <stdlib.h>

struct Env* new_env(struct Env* parent) {
    struct Env* env = malloc(sizeof(struct Env));
    env->vars = malloc(sizeof(struct VarList));
    env->vars->vars = NULL;
    env->vars->count = 0;
    env->vars->capacity = 0;
    env->parent = parent;
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
    if (env->vars)
        free_var_list(env->vars);
    free(env);
}
