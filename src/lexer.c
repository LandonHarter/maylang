#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
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

int follows(struct LexerSource* src, const char* match) {
    for (size_t i = 0; i < strlen(match); i++) {
        if (src->src[src->idx + i + 1] != match[i]) {
            return 0;
        }
    }

    src->idx += strlen(match);
    return 1;
}

struct TokenList tokenize(struct Source* filesource) {
    char* src = filesource->src;
    struct TokenList list = {NULL, 0, 0};
    struct LexerSource source = {src, 0, 1};

    while (source.src[source.idx] != '\0') {
        char c = src[source.idx];
        struct Token tok;
        tok.line = source.line;

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
            case '+':
                if (follows(&source, "=")) {
                    tok.type = PLUS_EQUALS;
                    tok.lexeme[0] = '+';
                    tok.lexeme[1] = '=';
                    tok.lexeme[2] = '\0';
                    append_token(&list, tok);
                } else if (follows(&source, "+")) {
                    tok.type = PLUS_PLUS;
                    tok.lexeme[0] = '+';
                    tok.lexeme[1] = '+';
                    tok.lexeme[2] = '\0';
                    append_token(&list, tok);
                } else {
                    append_basic_token(&list, &tok, PLUS, &c);
                }
                break;
            case '-':
                if (follows(&source, "=")) {
                    tok.type = MINUS_EQUALS;
                    tok.lexeme[0] = '-';
                    tok.lexeme[1] = '=';
                    tok.lexeme[2] = '\0';
                    append_token(&list, tok);
                } else if (follows(&source, "-")) {
                    tok.type = MINUS_MINUS;
                    tok.lexeme[0] = '-';
                    tok.lexeme[1] = '-';
                    tok.lexeme[2] = '\0';
                    append_token(&list, tok);
                } else if (follows(&source, ">")) {
                    tok.type = ARROW;
                    tok.lexeme[0] = '-';
                    tok.lexeme[1] = '>';
                    tok.lexeme[2] = '\0';
                    append_token(&list, tok);
                } else {
                    append_basic_token(&list, &tok, MINUS, &c);
                }
                break;
            case '*':
                if (follows(&source, "=")) {
                    tok.type = MULTIPLY_EQUALS;
                    tok.lexeme[0] = '*';
                    tok.lexeme[1] = '=';
                    tok.lexeme[2] = '\0';
                    append_token(&list, tok);
                } else {
                    append_basic_token(&list, &tok, MULTIPLY, &c);
                }
                break;
            case '/':
                if (follows(&source, "=")) {
                    tok.type = DIVIDE_EQUALS;
                    tok.lexeme[0] = '/';
                    tok.lexeme[1] = '=';
                    tok.lexeme[2] = '\0';
                    append_token(&list, tok);
                } else {
                    append_basic_token(&list, &tok, DIVIDE, &c);
                }
                break;
            case '%':
                append_basic_token(&list, &tok, MODULO, &c);
                break;
            case '>':
                if (follows(&source, "=")) {
                    tok.type = GTE;
                    tok.lexeme[0] = '>';
                    tok.lexeme[0] = '=';
                    tok.lexeme[0] = '\0';
                    append_token(&list, tok);
                } else {
                    append_basic_token(&list, &tok, GT, &c);
                }
                break;
            case '<':
                if (follows(&source, "=")) {
                    tok.type = LTE;
                    tok.lexeme[0] = '<';
                    tok.lexeme[0] = '=';
                    tok.lexeme[0] = '\0';
                    append_token(&list, tok);
                } else {
                    append_basic_token(&list, &tok, LT, &c);
                }
                break;
            case '!':
                if (follows(&source, "=")) {
                    tok.type = NOT_EQUALS;
                    tok.lexeme[0] = '!';
                    tok.lexeme[1] = '=';
                    tok.lexeme[2] = '\0';
                    append_token(&list, tok);
                } else {
                    tok.type = BANG;
                    tok.lexeme[0] = '!';
                    tok.lexeme[1] = '\0';
                    append_token(&list, tok);
                }
                break;
            case '?':
                append_basic_token(&list, &tok, QUESTION, &c);
                break;
            case ',':
                append_basic_token(&list, &tok, COMMA, &c);
                break;
            case ';':
                append_basic_token(&list, &tok, SEMICOLON, &c);
                break;
            case ':':
                append_basic_token(&list, &tok, COLON, &c);
                break;
            case '&':
                if (follows(&source, "&")) {
                    tok.type = AND;
                    tok.lexeme[0] = '&';
                    tok.lexeme[1] = '&';
                    tok.lexeme[2] = '\0';
                    append_token(&list, tok);
                }
                break;
            case '|':
                if (follows(&source, "|")) {
                    tok.type = OR;
                    tok.lexeme[0] = '|';
                    tok.lexeme[1] = '|';
                    tok.lexeme[2] = '\0';
                    append_token(&list, tok);
                } else {
                    tok.type = PIPE;
                    tok.lexeme[0] = '|';
                    tok.lexeme[1] = '\0';
                    append_token(&list, tok);
                }
                break;
            case '=':
                if (follows(&source, "=")) {
                    tok.type = EQUALS_EQUALS;
                    tok.lexeme[0] = '=';
                    tok.lexeme[1] = '=';
                    tok.lexeme[2] = '\0';
                } else {
                    tok.type = EQUALS;
                    tok.lexeme[0] = '=';
                    tok.lexeme[1] = '\0';
                }

                append_token(&list, tok);
                break;
            case '"':;
                struct MayValue* str = malloc(sizeof(struct MayValue));
                str->type = MAY_STRING;
                string_val(&source, str);

                tok.type = STRING;
                tok.lexeme[0] = '"';
                tok.lexeme[1] = '"';
                tok.lexeme[2] = '\0';
                tok.literal = str;

                append_token(&list, tok);
                break;
            case '\n':
                source.line += 1;
                break;
            case ' ': break;
            case '\t': break;
            case '\r': break;
            case '(':
                append_basic_token(&list, &tok, LEFT_PAREN, &c);
                break;
            case ')':
                append_basic_token(&list, &tok, RIGHT_PAREN, &c);
                break;
            default:
                if (is_digit(c)) {
                    struct MayValue* num = malloc(sizeof(struct MayValue));
                    num->type = MAY_FLOAT;
                    number_val(&source, num, &tok);

                    tok.type = NUMBER;
                    tok.literal = num;

                    append_token(&list, tok);
                } else if (is_alpha(c)) {
                    identifier_val(&source, &tok);
                    append_token(&list, tok);
                } else {
                    perror("Unknown character");
                    free_token_list(&list);
                    exit(-1);
                }
                break;
        }

        source.idx += 1;
    }

    struct Token eof_tok;
    eof_tok.type = END_FILE;
    eof_tok.line = source.line;
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

void string_val(struct LexerSource* src, struct MayValue* str) {
    src->idx++;
    int start = src->idx;

    int end = start;
    while (src->src[end] != '"' && src->src[end] != '\0') {
        if (src->src[end] == '\\' && src->src[end + 1] != '\0') {
            end += 2;
        } else {
            end++;
        }
    }

    char* val = malloc(end - start + 1);
    int out = 0;
    int i = start;
    while (i < end) {
        if (src->src[i] == '\\' && i + 1 < end) {
            char next = src->src[i + 1];
            switch (next) {
                case 'n': val[out++] = '\n'; break;
                case 't': val[out++] = '\t'; break;
                case 'r': val[out++] = '\r'; break;
                case '\\': val[out++] = '\\'; break;
                case '"': val[out++] = '"'; break;
                case '0': val[out++] = '\0'; break;
                default:
                    val[out++] = '\\';
                    val[out++] = next;
                    break;
            }
            i += 2;
        } else {
            val[out++] = src->src[i];
            i++;
        }
    }
    val[out] = '\0';
    str->as.string.val = val;
    str->as.string.len = out;
    src->idx = end;
}

void number_val(struct LexerSource* src, struct MayValue* num, struct Token* tok) {
    int len = 0;
    while (is_digit_or_dec(src->src[src->idx++])) {
        len++;
    }

    src->idx -= 2;

    char digits[len + 1];
    strncpy(digits, src->src + src->idx - len + 1, len);
    digits[len] = '\0';
    num->as.floating = (double) atof(digits);

    for (int i = 0; i < len + 1; i++) {
        tok->lexeme[i] = digits[i];
    }
}

void identifier_val(struct LexerSource* src, struct Token* tok) {
    unsigned int len = 0;
    while (is_alphanumeric(src->src[src->idx++])) {
        len++;
    }

    src->idx -= 2;

    tok->type = IDENTIFIER;
    for (int i = 0; i < len; i++) {
        tok->lexeme[i] = src->src[src->idx - len + 1 + i];
    }
    tok->lexeme[len] = '\0';

    get_keyword(src, tok);
}
