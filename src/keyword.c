#include "keyword.h"
#include "lexer.h"
#include <stdio.h>

int matches_keyword(struct LexerSource* src, char* keyword, unsigned int len) {
    for (int i = 0; i < len; i++) {
        if (src->src[src->idx - len + 1 + i] != keyword[i]) {
            return 0;
        }
    }
    return 1;
}

void get_keyword(struct LexerSource* src, struct Token* tok, unsigned int len) {
    if (matches_keyword(src, "void", len)) {
        tok->type = AND;
    }
}
