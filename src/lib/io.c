#include "types.h"
#include "env.h"
#include "error.h"
#include "module.h"
#include "lib/io.h"
#include <stdlib.h>

struct MayValue* io_print(struct MayValue** args, int arg_count) {
    printmv(args[0]);
    return NULL;
}

void load_io_env(struct Env* env) {
    enum MayValueType* arg_types = malloc(sizeof(enum MayValueType) * 1);
    arg_types[0] = MAY_VAR;
    register_builtin(env, "print", io_print, 1, arg_types);
}

struct Module* load_io_module() {
    struct Module* mod = malloc(sizeof(struct Module));
    mod->name = "io";
    mod->loader = load_io_env;
    return mod;
}
