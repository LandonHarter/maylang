#include "types.h"
#include "env.h"
#include "module.h"
#include "lib/fs.h"
#include <stdlib.h>

struct MayValue* fs_read_file(struct MayValue** args, int arg_count) {
    return NULL;
}

void load_fs_env(struct Env* env) {
    enum MayValueType* atread = malloc(sizeof(enum MayValueType) * 1);
    atread[0] = MAY_STRING;
    register_builtin(env, "read_file", fs_read_file, 1, atread);
}

struct Module* load_fs_module() {
    struct Module* mod = malloc(sizeof(struct Module));
    mod->name = "fs";
    mod->loader = load_fs_env;
    return mod;
}
