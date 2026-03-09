#include "module.h"
#include "env.h"
#include "runtime.h"
#include "error.h"
#include "debug.h"
#include <stdlib.h>

void register_module(struct ModuleList* list, struct Module* module) {
    if (list->count >= list->capacity) {
        list->capacity = list->capacity == 0 ? 8 : list->capacity * 2;
        list->modules = realloc(list->modules, list->capacity * sizeof(struct Module*));
    }
    list->modules[list->count++] = module;
}

void free_module_list(struct ModuleList* list) {
    free(list->modules);
    list->modules = NULL;
    list->count = 0;
    list->capacity = 0;
}

int compare_names(char* name1, char* name2) {
    int i = 0;
    while (name1[i] != '\0') {
        if (name1[i] != name2[i]) return 0;
        i++;
    }
    return 1;
}

void load_module(struct Env* env, char* name) {
    for (int i = 0; i < env->modules->count; i++) {
        if (compare_names(env->modules->modules[i]->name, name)) {
            env->modules->modules[i]->loader(env);
            return;
        }
    }

    debug_error("Failed to laod module %s", name);
    throw_runtime_error("Module doesn't exist");
}
