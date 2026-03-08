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
    struct Env* parent;
};

struct Env* new_env(struct Env* parent);
void free_env(struct Env* env);
