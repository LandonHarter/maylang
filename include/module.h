#include <stddef.h>

struct Env;
typedef void (*MayModuleLoader)(struct Env* env);
struct Module {
    char* name;
    MayModuleLoader loader;
};

struct ModuleList {
    struct Module** modules;
    size_t capacity;
    size_t count;
};

void register_module(struct ModuleList* list, struct Module* module);
void free_module_list(struct ModuleList* list);
void load_module(struct Env* env, char* name);
