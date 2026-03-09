#include "keyword.h"
#include "lexer.h"
#include <string.h>

int matches_keyword(char* identifier, char* keyword) {
    for (int i = 0; i < strlen(identifier); i++) {
        if (identifier[i] != keyword[i]) {
            return 0;
        }
    }
    return 1;
}

void get_keyword(struct LexerSource* src, struct Token* tok) {
    if (matches_keyword(tok->lexeme, "var")) {
        tok->type = VAR;
    } else if (matches_keyword(tok->lexeme, "float")) {
        tok->type = FLOAT;
    } else if (matches_keyword(tok->lexeme, "int")) {
        tok->type = INT;
    } else if (matches_keyword(tok->lexeme, "return")) {
        tok->type = RETURN;
    } else if (matches_keyword(tok->lexeme, "import")) {
        tok->type = IMPORT;
    } else if (matches_keyword(tok->lexeme, "if")) {
        tok->type = IF;
    }
}
