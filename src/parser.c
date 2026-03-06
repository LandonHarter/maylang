#include "parser.h"
#include "lexer.h"

struct Expr* expr_number(float value) {
    struct Expr* e = malloc(sizeof(struct Expr));
    e->type = EXPR_NUMBER;
    e->as.number = value;
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
    }
    free(expr);
}

static struct Token* peek(struct Parser* p) {
    return &p->tokens[p->current];
}

static struct Token* advance(struct Parser* p) {
    return &p->tokens[p->current++];
}

static int check(struct Parser* p, enum TokenType type) {
    return peek(p)->type == type;
}

static struct Expr* parse_expression(struct Parser* p);

static struct Expr* parse_primary(struct Parser* p) {
    if (check(p, NUMBER)) {
        struct Token* tok = advance(p);
        float val = atof(tok->lexeme);
        return expr_number(val);
    }

    if (check(p, LEFT_PAREN)) {
        advance(p);
        struct Expr* inner = parse_expression(p);
        if (!check(p, RIGHT_PAREN)) {
            fprintf(stderr, "Expected ')' on line %u\n", peek(p)->line);
            exit(-1);
        }
        advance(p);
        return inner;
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

struct Expr* parse(struct TokenList* list) {
    struct Parser p = {list->tokens, list->count, 0};
    struct Expr* expr = parse_expression(&p);
    return expr;
}
