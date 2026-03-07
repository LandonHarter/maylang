#include <stddef.h>

struct MayValue;
struct Var {
    char* name;
    struct MayValue* value;
};

struct VarList {
    struct Var** vars;
    size_t count;
    size_t capacity;
};

struct Env {
    struct VarList* vars;
};

void free_env(struct Env* env);
