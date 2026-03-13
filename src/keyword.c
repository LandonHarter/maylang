#include "keyword.h"
#include "lexer.h"
#include <string.h>

int matches_keyword(char* identifier, char* keyword) {
    if (strlen(identifier) != strlen(keyword)) {
        return 0;
    }
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
    } else if (matches_keyword(tok->lexeme, "export")) {
        tok->type = EXPORT;
    } else if (matches_keyword(tok->lexeme, "if")) {
        tok->type = IF;
    } else if (matches_keyword(tok->lexeme, "else")) {
        tok->type = ELSE;
    } else if (matches_keyword(tok->lexeme, "while")) {
        tok->type = WHILE;
    } else if (matches_keyword(tok->lexeme, "string")) {
        tok->type = STRING_TYPE;
    } else if (matches_keyword(tok->lexeme, "object")) {
        tok->type = OBJECT_TYPE;
    } else if (matches_keyword(tok->lexeme, "array")) {
        tok->type = ARRAY_TYPE;
    }
}
