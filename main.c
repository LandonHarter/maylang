#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

int main(int argc, char* argv[]) {
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

    struct TokenList tokens = tokenize(fbuf);
    free(fbuf);

    for (size_t i = 0; i < tokens.count; i++) {
        printf("Token { type: %d, lexeme: \"%s\", line: %u }\n",
               tokens.tokens[i].type, tokens.tokens[i].lexeme, tokens.tokens[i].line);
    }

    free_token_list(&tokens);

    return 0;
}
