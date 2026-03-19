#include "types.h"
#include "env.h"
#include "module.h"
#include "lib/fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

struct MayValue* fs_read_file(struct MayValue** args, int arg_count) {
    FILE* f = fopen(args[0]->as.string.val, "r");
    if (!f) {
        struct MayValue* ret = malloc(sizeof(struct MayValue));
        ret->type = MAY_STRING;
        ret->as.string.val = strdup("");
        ret->as.string.len = 0;
        return ret;
    }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buf = malloc(len + 1);
    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);

    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_STRING;
    ret->as.string.val = buf;
    ret->as.string.len = len;
    return ret;
}

struct MayValue* fs_write_file(struct MayValue** args, int arg_count) {
    FILE* f = fopen(args[0]->as.string.val, "w");
    if (f) {
        fwrite(args[1]->as.string.val, 1, args[1]->as.string.len, f);
        fclose(f);
    }

    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = (f != NULL);
    return ret;
}

struct MayValue* fs_append_file(struct MayValue** args, int arg_count) {
    FILE* f = fopen(args[0]->as.string.val, "a");
    if (f) {
        fwrite(args[1]->as.string.val, 1, args[1]->as.string.len, f);
        fclose(f);
    }

    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = (f != NULL);
    return ret;
}

struct MayValue* fs_file_exists(struct MayValue** args, int arg_count) {
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = (access(args[0]->as.string.val, F_OK) == 0);
    return ret;
}

struct MayValue* fs_delete_file(struct MayValue** args, int arg_count) {
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = (remove(args[0]->as.string.val) == 0);
    return ret;
}

struct MayValue* fs_rename_file(struct MayValue** args, int arg_count) {
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = (rename(args[0]->as.string.val, args[1]->as.string.val) == 0);
    return ret;
}

struct MayValue* fs_mkdir(struct MayValue** args, int arg_count) {
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = (mkdir(args[0]->as.string.val, 0755) == 0);
    return ret;
}

struct MayValue* fs_list_dir(struct MayValue** args, int arg_count) {
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_ARRAY;

    DIR* dir = opendir(args[0]->as.string.val);
    if (!dir) {
        ret->as.array.elements = NULL;
        ret->as.array.length = 0;
        return ret;
    }

    int capacity = 16;
    int count = 0;
    struct MayValue** elements = malloc(sizeof(struct MayValue*) * capacity);

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (count >= capacity) {
            capacity *= 2;
            elements = realloc(elements, sizeof(struct MayValue*) * capacity);
        }

        struct MayValue* item = malloc(sizeof(struct MayValue));
        item->type = MAY_STRING;
        item->as.string.val = strdup(entry->d_name);
        item->as.string.len = strlen(entry->d_name);
        elements[count++] = item;
    }
    closedir(dir);

    ret->as.array.elements = elements;
    ret->as.array.length = count;
    return ret;
}

struct MayValue* fs_is_dir(struct MayValue** args, int arg_count) {
    struct stat st;
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    ret->as.floating = (stat(args[0]->as.string.val, &st) == 0 && S_ISDIR(st.st_mode));
    return ret;
}

struct MayValue* fs_file_size(struct MayValue** args, int arg_count) {
    struct stat st;
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_FLOAT;
    if (stat(args[0]->as.string.val, &st) == 0) {
        ret->as.floating = (float)st.st_size;
    } else {
        ret->as.floating = -1;
    }
    return ret;
}

void load_fs_env(struct Env* env) {
    enum MayValueType* one_string = malloc(sizeof(enum MayValueType) * 1);
    one_string[0] = MAY_STRING;

    register_builtin(env, "read_file", fs_read_file, 1, one_string);
    register_builtin(env, "file_exists", fs_file_exists, 1, one_string);
    register_builtin(env, "delete_file", fs_delete_file, 1, one_string);
    register_builtin(env, "mkdir", fs_mkdir, 1, one_string);
    register_builtin(env, "list_dir", fs_list_dir, 1, one_string);
    register_builtin(env, "is_dir", fs_is_dir, 1, one_string);
    register_builtin(env, "file_size", fs_file_size, 1, one_string);

    enum MayValueType* two_strings = malloc(sizeof(enum MayValueType) * 2);
    two_strings[0] = MAY_STRING;
    two_strings[1] = MAY_STRING;

    register_builtin(env, "write_file", fs_write_file, 2, two_strings);
    register_builtin(env, "append_file", fs_append_file, 2, two_strings);
    register_builtin(env, "rename_file", fs_rename_file, 2, two_strings);
}

struct Module* load_fs_module() {
    struct Module* mod = malloc(sizeof(struct Module));
    mod->name = "fs";
    mod->loader = load_fs_env;
    return mod;
}
