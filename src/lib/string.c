#include "lib/string.h"
#include "types.h"
#include "env.h"
#include "error.h"
#include "module.h"
#include <stdio.h>
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

struct MayValue* string_lower(struct MayValue** args, int arg_count) {
    char* lowerstr = strdup(args[0]->as.string.val);

    for (int i = 0; lowerstr[i] != '\0'; i++) {
        lowerstr[i] = (char)tolower((unsigned char)lowerstr[i]);
    }

    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_STRING;
    ret->as.string.val = lowerstr;
    ret->as.string.len = strlen(lowerstr);
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

struct MayValue* string_get_char(struct MayValue** args, int arg_count) {
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_STRING;
    ret->as.string.val = malloc(2);
    ret->as.string.val[0] = args[0]->as.string.val[(int)args[1]->as.floating];
    ret->as.string.val[1] = '\0';
    ret->as.string.len = 1;
    return ret;
}

struct MayValue* string_replace(struct MayValue** args, int arg_count) {
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_STRING;

    char* original = args[0]->as.string.val;
    int original_len = strlen(original);
    char* find = args[1]->as.string.val;
    int find_len = strlen(find);
    char* replace = args[2]->as.string.val;
    int replace_len = strlen(replace);

    char* returnstr;
    int count;
    int front_length;
    char* tmp;
    char* ins = (char*) original;

    for (count = 0; (tmp = strstr(ins, find)); count++) {
        ins = tmp + replace_len;
    }

    tmp = returnstr = malloc(original_len + (replace_len - find_len) * count + 1);
    while (count--) {
        ins = strstr(original, find);
        front_length = ins - original;
        tmp = strncpy(tmp, original, front_length) + front_length;
        tmp = strcpy(tmp, replace) + replace_len;
        original += front_length + find_len;
    }
    strcpy(tmp, original);

    ret->as.string.val = returnstr;
    ret->as.string.len = strlen(returnstr);

    return ret;
}

struct MayValue* string_numeric(struct MayValue** args, int arg_count) {
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    int isnumeric = 1;
    for (int i = 0; i < args[0]->as.string.len; i++) {
        if (!is_digit(args[0]->as.string.val[i])) {
            isnumeric = 0;
        }
    }
    ret->as.floating = isnumeric;
    return ret;
}

struct MayValue* string_alpha(struct MayValue** args, int arg_count) {
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    int isalpha = 1;
    for (int i = 0; i < args[0]->as.string.len; i++) {
        if (!is_alpha(args[0]->as.string.val[i])) {
            isalpha = 0;
        }
    }
    ret->as.floating = isalpha;
    return ret;
}

struct MayValue* string_alphanumeric(struct MayValue** args, int arg_count) {
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    int isalphanumeric = 1;
    for (int i = 0; i < args[0]->as.string.len; i++) {
        if (!is_alphanumeric(args[0]->as.string.val[i])) {
            isalphanumeric = 0;
        }
    }
    ret->as.floating = isalphanumeric;
    return ret;
}

struct MayValue* string_substring(struct MayValue** args, int arg_count) {
    char* str = args[0]->as.string.val;
    int start = (int) args[1]->as.floating;
    int end = (int) args[2]->as.floating;
    if (end < 0) {
        end = strlen(str) + end + 1;
    }

    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_STRING;

    char* returnstr = malloc(end - start);
    for (int i = 0; i < end - start; i++) {
        returnstr[i] = str[i + start];
    }
    ret->as.string.val = returnstr;

    return ret;
}

void load_string_env(struct Env* env) {
    enum MayValueType* one_string = malloc(sizeof(enum MayValueType) * 1);
    one_string[0] = MAY_STRING;

    register_builtin(env, "upper", string_upper, 1, one_string);
    register_builtin(env, "lower", string_lower, 1, one_string);
    register_builtin(env, "capitalize", string_capitalize, 1, one_string);
    register_builtin(env, "numeric", string_numeric, 1, one_string);
    register_builtin(env, "alpha", string_alpha, 1, one_string);
    register_builtin(env, "alphanumeric", string_alphanumeric, 1, one_string);

    enum MayValueType* string_int = malloc(sizeof(enum MayValueType) * 2);
    one_string[0] = MAY_STRING;
    one_string[1] = MAY_FLOAT;

    register_builtin(env, "char", string_get_char, 2, string_int);

    enum MayValueType* string_int_int = malloc(sizeof(enum MayValueType) * 2);
    string_int_int[0] = MAY_STRING;
    string_int_int[1] = MAY_FLOAT;
    string_int_int[2] = MAY_FLOAT;

    register_builtin(env, "substring", string_substring, 3, string_int_int);

    enum MayValueType* string_string_string = malloc(sizeof(enum MayValueType));
    string_string_string[0] = MAY_STRING;
    string_string_string[1] = MAY_STRING;
    string_string_string[2] = MAY_STRING;

    register_builtin(env, "replace", string_replace, 3, string_string_string);
}

struct Module* load_string_module() {
    struct Module* mod = malloc(sizeof(struct Module));
    mod->name = "string";
    mod->loader = load_string_env;
    return mod;
}
