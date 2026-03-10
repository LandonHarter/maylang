#include "types.h"
#include "env.h"
#include "error.h"
#include "module.h"
#include "lib/io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MayValue* dotenv_envvar(struct MayValue** args, int arg_count) {
    char* fpth = ".env";
    FILE* fptr;
    fptr = fopen(fpth, "r");
    if (fptr == NULL) {
        throw_runtime_error("Failed to open .env file");
    }

    fseek(fptr, 0L, SEEK_END);
    unsigned long fsize = ftell(fptr);
    rewind(fptr);
    char* fbuf = (char*) malloc(fsize + 1);
    size_t fbuf_read = fread(fbuf, 1, fsize, fptr);
    if (fbuf_read != fsize) {
        free(fbuf);
        fclose(fptr);
        throw_runtime_error("Error reading file");
    }
    fbuf[fsize] = '\0';
    fclose(fptr);

    char* token = strtok(fbuf, "\n");
    while (token != NULL) {
        int line_len = strlen(token);
        char* name = malloc(line_len + 1);
        char* value = malloc(line_len + 1);

        int found_equals = 0;
        int name_idx = 0;
        int value_idx = 0;
        for (int i = 0; i < line_len; i++) {
            if (!found_equals && token[i] == '=') {
                found_equals = 1;
                continue;
            } else if (!found_equals) {
                name[name_idx] = token[i];
                name_idx++;
            } else if (token[i] != '"') {
                value[value_idx] = token[i];
                value_idx++;
            }
        }
        name[name_idx] = '\0';
        value[value_idx] = '\0';

        if (strcmp(name, args[0]->as.string.val) == 0) {
            struct MayValue* ret = malloc(sizeof(struct MayValue));
            ret->type = MAY_STRING;
            ret->as.string.val = value;
            ret->as.string.len = strlen(value);
            return ret;
        }

        token = strtok(NULL, "\n");
    }

    return NULL;
}

void load_dotenv_env(struct Env* env) {
    enum MayValueType* str = malloc(sizeof(enum MayValueType) * 1);
    str[0] = MAY_STRING;

    register_builtin(env, "envvar", dotenv_envvar, 1, str);
}

struct Module* load_dotenv_module() {
    struct Module* mod = malloc(sizeof(struct Module));
    mod->name = "dotenv";
    mod->loader = load_dotenv_env;
    return mod;
}
