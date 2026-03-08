#include <stddef.h>

struct MayValue;
typedef struct MayValue* (*MayNativeFn)(struct MayValue** args, int arg_count);
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
void append_var(struct Env* env, struct Var* var);
void register_builtin(struct Env* env, const char* name, MayNativeFn fn);
