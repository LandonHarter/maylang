#include "env.h"
#include <stdlib.h>

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
