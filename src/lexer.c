#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lexer.h"

static void append_token(struct TokenList* list, struct Token token) {
    if (list->count >= list->capacity) {
        list->capacity = list->capacity == 0 ? 8 : list->capacity * 2;
        list->tokens = realloc(list->tokens, list->capacity * sizeof(struct Token));
    }
    list->tokens[list->count++] = token;
}

static void append_basic_token(struct TokenList* list, struct Token* tok, enum TokenType tok_type, char* c) {
    tok->type = tok_type;
    tok->lexeme[0] = *c;
    tok->lexeme[1] = '\0';
    append_token(list, *tok);
}

static bool follows(char* src, int* idx, const char* match) {
    for (size_t i = 0; i < strlen(match); i++) {
        if (src[*idx + i + 1] != match[i]) {
            return false;
        }
    }

    *idx += strlen(match);
    return true;
}

struct TokenList tokenize(char* src) {
    struct TokenList list = {NULL, 0, 0};
    unsigned int line = 1;

    for (int i = 0; src[i] != '\0'; i++) {
        char c = src[i];
        struct Token tok;
        tok.line = line;

        switch (c) {
            case '[':
                append_basic_token(&list, &tok, LEFT_BRACKET, &c);
                break;
            case ']':
                append_basic_token(&list, &tok, RIGHT_BRACKET, &c);
                break;
            case '{':
                append_basic_token(&list, &tok, LEFT_BRACE, &c);
                break;
            case '}':
                append_basic_token(&list, &tok, RIGHT_BRACE, &c);
                break;
            case '!':
                append_basic_token(&list, &tok, EXCLAMATION, &c);
                break;
            case '?':
                append_basic_token(&list, &tok, QUESTION, &c);
                break;
            case ';':
                append_basic_token(&list, &tok, SEMICOLON, &c);
                break;
            case '=':
                int num_equal = 1;
                if (follows(src, &i, "===")) {
                    tok.type = EQUALS_EQUALS_EQUALS_EQUALS;
                    num_equal = 4;
                } else if (follows(src, &i, "==")) {
                    tok.type = EQUALS_EQUALS_EQUALS;
                    num_equal = 3;
                } else if (follows(src, &i, "=")) {
                    tok.type = EQUALS_EQUALS;
                    num_equal = 2;
                }

                for (int j = 0; j < num_equal; j++) {
                    tok.lexeme[j] = '=';
                }
                tok.lexeme[num_equal] = '\0';
                append_token(&list, tok);
                break;
            case '\n':
                line++;
                break;
            case ' ': break;
            case '\t': break;
            case '\r': break;
            case '(': break;
            case ')': break;
            default:
                perror("Unknown character");
                free_token_list(&list);
                exit(-1);
                break;
        }
    }

    struct Token eof_tok;
    eof_tok.type = END_FILE;
    eof_tok.line = line;
    eof_tok.lexeme[0] = '\0';
    append_token(&list, eof_tok);

    return list;
}

void free_token_list(struct TokenList* list) {
    free(list->tokens);
    list->tokens = NULL;
    list->count = 0;
    list->capacity = 0;
}
