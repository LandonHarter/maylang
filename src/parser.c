#include "parser.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Expr* expr_number(float value) {
    struct Expr* e = malloc(sizeof(struct Expr));
    e->type = EXPR_NUMBER;
    e->as.number = value;
    return e;
}

struct Expr* expr_string(char* value) {
    struct Expr* e = malloc(sizeof(struct Expr));
    e->type = EXPR_STRING;
    e->as.string = value;
    return e;
}

struct Expr* expr_variable(char* name) {
    struct Expr* e = malloc(sizeof(struct Expr));
    e->type = EXPR_VARIABLE;
    e->as.string = name;
    return e;
}

struct Expr* expr_vardecl(char* name, struct Expr* initializer) {
    struct Expr* e = malloc(sizeof(struct Expr));
    e->type = EXPR_VARIABLEDECL;
    e->as.vardecl.name = name;
    e->as.vardecl.initializer = initializer;
    return e;
}

struct Expr* expr_binary(struct Expr* left, struct Expr* right, char op) {
    struct Expr* e = malloc(sizeof(struct Expr));
    e->type = EXPR_BINARY;
    e->as.binary.left = left;
    e->as.binary.right = right;
    e->as.binary.op = op;
    return e;
}

struct Expr* expr_unary(char op, struct Expr* operand) {
    struct Expr* e = malloc(sizeof(struct Expr));
    e->type = EXPR_UNARY;
    e->as.unary.op = op;
    e->as.unary.operand = operand;
    return e;
}

void expr_free(struct Expr* expr) {
    if (!expr) return;
    switch (expr->type) {
        case EXPR_BINARY:
            expr_free(expr->as.binary.left);
            expr_free(expr->as.binary.right);
            break;
        case EXPR_UNARY:
            expr_free(expr->as.unary.operand);
            break;
        case EXPR_NUMBER:
            break;
        case EXPR_STRING:
            break;
        case EXPR_VARIABLE:
            break;
        case EXPR_VARIABLEDECL:
            expr_free(expr->as.vardecl.initializer);
            free(expr->as.vardecl.name);
            break;
    }
    free(expr);
}

void stmt_free(struct Stmt* stmt) {
    if (!stmt) return;
    switch (stmt->type) {
        case STMT_EXPR:
        case STMT_VARDECL:
            expr_free(stmt->as.expr);
            break;
    }
    free(stmt);
}

static struct Token* peek(struct Parser* p) {
    return &p->tokens[p->current];
}

static struct Token* advance(struct Parser* p) {
    return &p->tokens[p->current++];
}

static struct Token* previous(struct Parser* p) {
    return &p->tokens[p->current--];
}

static int check(struct Parser* p, enum TokenType type) {
    return peek(p)->type == type;
}

static int matches(char lexeme[], char* match) {
    for (int i = 0; i  < 256; i++) {
        if (lexeme[i] == '\0') break;
        if (lexeme[i] != match[i]) return 0;
    }
    return 1;
}

static struct Expr* parse_expression(struct Parser* p);

static struct Expr* parse_primary(struct Parser* p) {
    if (check(p, NUMBER)) {
        struct Token* tok = advance(p);
        float val = atof(tok->lexeme);
        return expr_number(val);
    } if (check(p, STRING)) {
        struct Token* tok = advance(p);
        int len = strlen(tok->lexeme) - 2;
        char* val = malloc(len + 1);
        strncpy(val, tok->lexeme + 1, len);
        val[len] = '\0';
        return expr_string(val);
    } if (check(p, LEFT_PAREN)) {
        advance(p);
        struct Expr* inner = parse_expression(p);
        if (!check(p, RIGHT_PAREN)) {
            fprintf(stderr, "Expected ')' on line %u\n", peek(p)->line);
            exit(-1);
        }
        advance(p);
        return inner;
    } if (check(p, VAR)) {
        advance(p);
        struct Token* name = advance(p);

        if (name->type != IDENTIFIER) {
            printf("Cannot name variable %s\n", name->lexeme);
            exit(-1);
        }
        int name_len = strlen(name->lexeme);
        char* name_val = malloc(name_len + 1);
        strncpy(name_val, name->lexeme, name_len);
        name_val[name_len] = '\0';

        struct Expr* initializer = NULL;
        if (check(p, EQUALS)) {
            advance(p);
            initializer = parse_expression(p);
        }

        return expr_vardecl(name_val, initializer);
    } if (check(p, IDENTIFIER)) {
        struct Token* tok = advance(p);
        int len = strlen(tok->lexeme);
        char* name = malloc(len + 1);
        strncpy(name, tok->lexeme, len);
        name[len] = '\0';
        return expr_variable(name);
    }

    fprintf(stderr, "Unexpected token '%s' on line %u\n", peek(p)->lexeme, peek(p)->line);
    exit(-1);
}

static struct Expr* parse_unary(struct Parser* p) {
    if (check(p, MINUS)) {
        advance(p);
        struct Expr* operand = parse_unary(p);
        return expr_unary('-', operand);
    } else if (check(p, BANG)) {
        advance(p);
        struct Expr* operand= parse_unary(p);
        return expr_unary('!', operand);
    }
    return parse_primary(p);
}

static struct Expr* parse_multiplicative(struct Parser* p) {
    struct Expr* left = parse_unary(p);

    while (check(p, MULTIPLY) || check(p, DIVIDE)) {
        char op = advance(p)->lexeme[0];
        struct Expr* right = parse_unary(p);
        left = expr_binary(left, right, op);
    }

    return left;
}

static struct Expr* parse_additive(struct Parser* p) {
    struct Expr* left = parse_multiplicative(p);

    while (check(p, PLUS) || check(p, MINUS)) {
        char op = advance(p)->lexeme[0];
        struct Expr* right = parse_multiplicative(p);
        left = expr_binary(left, right, op);
    }

    return left;
}

static struct Expr* parse_expression(struct Parser* p) {
    return parse_additive(p);
}

static struct Stmt* parse_statement(struct Parser* p, struct Env* env) {
    struct Stmt* stmt = malloc(sizeof(struct Stmt));

    if (check(p, VAR)) {
        stmt->type = STMT_VARDECL;
    } else {
        stmt->type = STMT_EXPR;
    }
    stmt->as.expr = parse_expression(p);

    if (!check(p, SEMICOLON)) {
        fprintf(stderr, "Expected semicolon on line %i", peek(p)->line - 1);
        exit(-1);
    }
    advance(p);

    return stmt;
}

struct StmtList parse(struct TokenList* list, struct Env* env) {
    struct Parser p = {list->tokens, list->count, 0};
    struct StmtList stmts = {NULL, 0, 0};
    while (!check(&p, END_FILE)) {
        append_stmt(&stmts, parse_statement(&p, env));
    }

    return stmts;
}
