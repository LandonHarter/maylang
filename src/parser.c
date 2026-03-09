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

struct Expr* expr_vardecl(char* name, struct Expr* initializer, int decl_type) {
    struct Expr* e = malloc(sizeof(struct Expr));
    e->type = EXPR_VARIABLEDECL;
    e->as.vardecl.name = name;
    e->as.vardecl.initializer = initializer;
    e->as.vardecl.decl_type = decl_type;
    return e;
}

struct Expr* expr_funcdecl(char* name, int decl_type, struct StmtList* body, struct Param* params, int param_count) {
    struct Expr* e = malloc(sizeof(struct Expr));
    e->type = EXPR_FUNCDECL;
    e->as.funcdecl.name = name;
    e->as.funcdecl.decl_type = decl_type;
    e->as.funcdecl.func = body;
    e->as.funcdecl.params = params;
    e->as.funcdecl.param_count = param_count;
    return e;
}

struct Expr* expr_return(struct Expr* val) {
    struct Expr* e = malloc(sizeof(struct Expr));
    e->type = EXPR_FUNCRETURN;
    e->as.funcreturn.expr = val;
    return e;
}

struct Expr* expr_import(char* lib) {
    struct Expr* e = malloc(sizeof(struct Expr));
    e->type = EXPR_IMPORT;
    e->as.import.lib = lib;
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
        case EXPR_IMPORT:
            break;
        case EXPR_VARIABLEDECL:
            expr_free(expr->as.vardecl.initializer);
            free(expr->as.vardecl.name);
            break;
        case EXPR_FUNCDECL:
            free(expr->as.funcdecl.name);
            if (expr->as.funcdecl.func) {
                free_stmt_list(expr->as.funcdecl.func);
                free(expr->as.funcdecl.func);
            }
            for (int i = 0; i < expr->as.funcdecl.param_count; i++)
                free(expr->as.funcdecl.params[i].name);
            free(expr->as.funcdecl.params);
            break;
        case EXPR_FUNC:
            break;
        case EXPR_CALL:
            free(expr->as.call.name);
            for (int i = 0; i < expr->as.call.arg_count; i++)
                expr_free(expr->as.call.args[i]);
            free(expr->as.call.args);
            break;
        case EXPR_FUNCRETURN:
            expr_free(expr->as.funcreturn.expr);
            break;
    }
    free(expr);
}

void stmt_free(struct Stmt* stmt) {
    if (!stmt) return;
    switch (stmt->type) {
        case STMT_EXPR:
        case STMT_VARDECL:
        case STMT_FUNCDECL:
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
static struct StmtList parse_func(struct Parser* p);

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
    } if (check(p, VAR) || check(p, INT) || check(p, FLOAT)) {
        struct Token* type_tok = advance(p);
        int decl_type = type_tok->type;
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
        } else if (check(p, LEFT_PAREN)) {
            advance(p);

            struct Param* params = NULL;
            int param_count = 0;
            int param_capacity = 0;

            if (!check(p, RIGHT_PAREN)) {
                do {
                    if (!check(p, VAR) && !check(p, INT) && !check(p, FLOAT)) {
                        printf("Expected parameter type on line %u\n", peek(p)->line);
                        exit(-1);
                    }
                    int ptype = advance(p)->type;

                    if (!check(p, IDENTIFIER)) {
                        printf("Expected parameter name on line %u\n", peek(p)->line);
                        exit(-1);
                    }
                    struct Token* pname = advance(p);

                    if (param_count >= param_capacity) {
                        param_capacity = param_capacity == 0 ? 4 : param_capacity * 2;
                        params = realloc(params, param_capacity * sizeof(struct Param));
                    }
                    params[param_count].name = strdup(pname->lexeme);
                    params[param_count].type = ptype;
                    param_count++;
                } while (check(p, COMMA) && advance(p));
            }

            if (!check(p, RIGHT_PAREN)) {
                printf("Expected ) on line %u\n", peek(p)->line);
                exit(-1);
            }
            advance(p);

            if (!check(p, LEFT_BRACE)) {
                printf("Expected { on line %u\n", peek(p)->line);
                exit(-1);
            }
            advance(p);

            struct StmtList body = parse_func(p);
            advance(p);
            struct StmtList* body_ptr = malloc(sizeof(struct StmtList));
            *body_ptr = body;
            return expr_funcdecl(name_val, decl_type, body_ptr, params, param_count);
        }

        return expr_vardecl(name_val, initializer, decl_type);
    } if (check(p, IDENTIFIER)) {
        struct Token* tok = advance(p);
        int len = strlen(tok->lexeme);
        char* name = malloc(len + 1);
        strncpy(name, tok->lexeme, len);
        name[len] = '\0';

        if (check(p, LEFT_PAREN)) {
            advance(p);
            struct Expr** args = NULL;
            int arg_count = 0;
            int arg_capacity = 0;

            if (!check(p, RIGHT_PAREN)) {
                do {
                    if (arg_count >= arg_capacity) {
                        arg_capacity = arg_capacity == 0 ? 4 : arg_capacity * 2;
                        args = realloc(args, arg_capacity * sizeof(struct Expr*));
                    }
                    args[arg_count++] = parse_expression(p);
                } while (check(p, COMMA) && advance(p));
            }

            if (!check(p, RIGHT_PAREN)) {
                fprintf(stderr, "Expected ')' on line %u\n", peek(p)->line);
                exit(-1);
            }
            advance(p);

            struct Expr* e = malloc(sizeof(struct Expr));
            e->type = EXPR_CALL;
            e->as.call.name = name;
            e->as.call.args = args;
            e->as.call.arg_count = arg_count;
            return e;
        }

        return expr_variable(name);
    } if (check(p, RETURN)) {
        advance(p);
        return expr_return(parse_expression(p));
    } if (check(p, IMPORT)) {
        advance(p);
        if (!check(p, STRING)) {
            fprintf(stderr, "Expected string on line %u\n", peek(p)->line);
            exit(-1);
        }

        struct Token* lib = peek(p);
        advance(p);
        return expr_import(lib->literal->as.string.val);
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

    if (check(p, VAR) || check(p, INT) || check(p, FLOAT)) {
        stmt->type = STMT_VARDECL;
    } else {
        stmt->type = STMT_EXPR;
    }
    stmt->as.expr = parse_expression(p);

    if (stmt->as.expr->type == EXPR_FUNCDECL) {
        stmt->type = STMT_FUNCDECL;
        return stmt;
    }

    if (!check(p, SEMICOLON)) {
        fprintf(stderr, "Expected semicolon on line %i", peek(p)->line);
        exit(-1);
    }
    advance(p);

    return stmt;
}

static struct StmtList parse_func(struct Parser* p) {
    struct StmtList stmts = {NULL, 0, 0};
    while (!check(p, RIGHT_BRACE)) {
        append_stmt(&stmts, parse_statement(p, NULL));
    }
    return stmts;
}

struct StmtList parse(struct TokenList* list, struct Env* env) {
    struct Parser p = {list->tokens, list->count, 0};
    struct StmtList stmts = {NULL, 0, 0};
    while (!check(&p, END_FILE)) {
        append_stmt(&stmts, parse_statement(&p, env));
    }

    return stmts;
}
