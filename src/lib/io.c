#include "types.h"
#include "env.h"
#include "error.h"
#include "module.h"
#include "lib/io.h"
#include <stdlib.h>
#include <string.h>

struct MayValue* io_print(struct MayValue** args, int arg_count) {
    printmv(args[0]);
    return NULL;
}

struct MayValue* io_print_color(struct MayValue** args, int arg_count) {
    printmv_col(args[0], args[1]->as.string.val);
    return NULL;
}

void load_io_env(struct Env* env) {
    enum MayValueType* atprint = malloc(sizeof(enum MayValueType) * 1);
    atprint[0] = MAY_VAR;
    register_builtin(env, "print", io_print, 1, atprint);

    enum MayValueType* atcolprint = malloc(sizeof(enum MayValueType) * 1);
    atcolprint[0] = MAY_VAR;
    atcolprint[1] = MAY_STRING;
    register_builtin(env, "colprint", io_print_color, 2, atcolprint);
}

struct Module* load_io_module() {
    struct Module* mod = malloc(sizeof(struct Module));
    mod->name = "io";
    mod->loader = load_io_env;
    return mod;
}
