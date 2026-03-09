#include "lib/string.h"
#include "types.h"
#include "env.h"
#include "error.h"
#include "module.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct MayValue* string_upper(struct MayValue** args, int arg_count) {
    char* upperstr = strdup(args[0]->as.string.val);

    for (int i = 0; upperstr[i] != '\0'; i++) {
        upperstr[i] = (char)toupper((unsigned char)upperstr[i]);
    }

    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_STRING;
    ret->as.string.val = upperstr;
    ret->as.string.len = strlen(upperstr);
    return ret;
}

struct MayValue* string_capitalize(struct MayValue** args, int arg_count) {
    char* upperstr = strdup(args[0]->as.string.val);

    for (int i = 0; upperstr[i] != '\0'; i++) {
        if (i == 0 || upperstr[i - 1] == ' ') {
            upperstr[i] = (char)toupper((unsigned char)upperstr[i]);
        }
    }

    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_STRING;
    ret->as.string.val = upperstr;
    ret->as.string.len = strlen(upperstr);
    return ret;
}

void load_string_env(struct Env* env) {
    enum MayValueType* atupper = malloc(sizeof(enum MayValueType) * 1);
    atupper[0] = MAY_STRING;
    register_builtin(env, "upper", string_upper, 1, atupper);

    enum MayValueType* atcapt = malloc(sizeof(enum MayValueType) * 1);
    atcapt[0] = MAY_STRING;
    register_builtin(env, "capitalize", string_capitalize, 1, atcapt);
}

struct Module* load_string_module() {
    struct Module* mod = malloc(sizeof(struct Module));
    mod->name = "string";
    mod->loader = load_string_env;
    return mod;
}
