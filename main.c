#include "may.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "source.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
    #if defined(_WIN32) || defined(_WIN64)
        fprintf(stderr, "WINDOWS USER DETECTED, MAY WILL NOT RUN");
        exit(-69);
    #endif

    FILE* fptr;
    fptr = fopen(argv[1], "r");
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

    struct Source source = {argv[1], fbuf};
    struct TokenList tokens = tokenize(&source);
    free(fbuf);

    struct Expr* ast = parse(&tokens);
    struct MayValue* result = evaluate(ast);
    if (result->type == MAY_FLOAT)
        printf("Result is %f\n", result->as.floating);
    else if (result->type == MAY_STRING)
        printf("Result is %s\n", result->as.string.val);

    expr_free(ast);
    free_token_list(&tokens);

    return 0;
}
