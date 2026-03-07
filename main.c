#include "may.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    #if defined(_WIN32) || defined(_WIN64)
        fprintf(stderr, "WINDOWS USER DETECTED, MAY WILL NOT RUN");
        exit(-69);
    #endif

    if (argc < 2) {
        fprintf(stderr, "Invalid arguments");
        return -1;
    }

    char* fpth = argv[argc - 1];
    struct Runtime* runtime = malloc(sizeof(struct Runtime));
    runtime->flags = 0;
    set_global_runtime(runtime);

    for (int i = 1; i < argc - 1; i++) {
        enable_runtime_flag(argv[i], runtime);
    }

    FILE* fptr;
    fptr = fopen(fpth, "r");
    if (fptr == NULL) {
        perror("Error opening file");
        return -1;
    }

    fseek(fptr, 0L, SEEK_END);
    unsigned long fsize = ftell(fptr);
    rewind(fptr);

    char* fbuf = (char*) malloc(fsize + 1);
    if (fbuf == NULL) {
        perror("Error allocating memory");
        fclose(fptr);
        return -1;
    }

    size_t fbuf_read = fread(fbuf, 1, fsize, fptr);
    if (fbuf_read != fsize) {
        perror("Error reading file");
        free(fbuf);
        fclose(fptr);
        return -1;
    }
    fbuf[fsize] = '\0';
    fclose(fptr);

    struct Source source = {fpth, fbuf};
    struct TokenList tokens = tokenize(&source);
    free(fbuf);

    struct Env* env = malloc(sizeof(struct Env));
    env->vars = malloc(sizeof(struct VarList));
    env->vars->vars = NULL;
    env->vars->count = 0;
    env->vars->capacity = 0;
    struct StmtList ast = parse(&tokens, env);
    evaluate(&ast, env);

    free_stmt_list(&ast);
    free_token_list(&tokens);
    free_env(env);
    free(runtime);

    return 0;
}
