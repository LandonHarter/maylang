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

struct Expr* expr_if(struct Expr* condition, struct StmtList* thenbody) {
    struct Expr* e = malloc(sizeof(struct Expr));
    e->type = EXPR_IF;
    e->as.ifcond.condition = condition;
    e->as.ifcond.thenbody = thenbody;
    return e;
}

struct Expr* expr_while(struct Expr* condition, struct StmtList* thenbody) {
    struct Expr* e = malloc(sizeof(struct Expr));
    e->type = EXPR_WHILE;
    e->as.whilecond.condition = condition;
    e->as.whilecond.thenbody = thenbody;
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
        case EXPR_IF:
            expr_free(expr->as.ifcond.condition);
            free_stmt_list(expr->as.ifcond.thenbody);
            break;
        case EXPR_WHILE:
            expr_free(expr->as.whilecond.condition);
            free_stmt_list(expr->as.whilecond.thenbody);
            break;
        case EXPR_ASSIGN:
            free(expr->as.assign.name);
            expr_free(expr->as.assign.value);
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
        case STMT_IF:
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
    } if (check(p, VAR) || check(p, INT) || check(p, FLOAT) || check(p, STRING_TYPE)) {
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
                    if (!check(p, VAR) && !check(p, INT) && !check(p, FLOAT) && !check(p, STRING_TYPE)) {
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

        if (check(p, EQUALS)) {
            advance(p);
            struct Expr* value = parse_expression(p);
            struct Expr* e = malloc(sizeof(struct Expr));
            e->type = EXPR_ASSIGN;
            e->as.assign.name = name;
            e->as.assign.value = value;
            return e;
        }

        if (check(p, PLUS_EQUALS) || check(p, MINUS_EQUALS) || check(p, MULTIPLY_EQUALS) || check(p, DIVIDE_EQUALS)) {
            char op;
            if (check(p, PLUS_EQUALS)) op = '+';
            else if (check(p, MINUS_EQUALS)) op = '-';
            else if (check(p, MULTIPLY_EQUALS)) op = '*';
            else op = '/';
            advance(p);
            struct Expr* rhs = parse_expression(p);
            struct Expr* var = expr_variable(strdup(name));
            struct Expr* bin = expr_binary(var, rhs, op);
            struct Expr* e = malloc(sizeof(struct Expr));
            e->type = EXPR_ASSIGN;
            e->as.assign.name = name;
            e->as.assign.value = bin;
            return e;
        }

        if (check(p, PLUS_PLUS) || check(p, MINUS_MINUS)) {
            char op = check(p, PLUS_PLUS) ? '+' : '-';
            advance(p);
            struct Expr* var = expr_variable(strdup(name));
            struct Expr* one = expr_number(1);
            struct Expr* bin = expr_binary(var, one, op);
            struct Expr* e = malloc(sizeof(struct Expr));
            e->type = EXPR_ASSIGN;
            e->as.assign.name = name;
            e->as.assign.value = bin;
            return e;
        }

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
    } if (check(p, IF)) {
        advance(p); 
        if (!check(p, LEFT_PAREN)) {
            fprintf(stderr, "Expected '(' on line %u\n", peek(p)->line);
            exit(-1);
        }

        struct Expr* condition = parse_expression(p);
        if (!check(p, LEFT_BRACE)) {
            fprintf(stderr, "Expected '{' on line %u\n", peek(p)->line);
            exit(-1);
        }
        advance(p);

        struct StmtList thenbody = parse_func(p);
        advance(p);
        struct StmtList* thenbody_ptr = malloc(sizeof(struct StmtList));
        *thenbody_ptr = thenbody;

        return expr_if(condition, thenbody_ptr);
    } if (check(p, WHILE)) {
        advance(p);
        if (!check(p, LEFT_PAREN)) {
            fprintf(stderr, "Expected '(' on line %u\n", peek(p)->line);
            exit(-1);
        }

        struct Expr* condition = parse_expression(p);
        if (!check(p, LEFT_BRACE)) {
            fprintf(stderr, "Expected '{' on line %u\n", peek(p)->line);
            exit(-1);
        }
        advance(p);

        struct StmtList thenbody = parse_func(p);
        advance(p);
        struct StmtList* thenbody_ptr = malloc(sizeof(struct StmtList));
        *thenbody_ptr = thenbody;
        return expr_while(condition, thenbody_ptr);
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

    while (check(p, MULTIPLY) || check(p, DIVIDE) || check(p, MODULO)) {
        char op = advance(p)->lexeme[0];
        struct Expr* right = parse_unary(p);
        left = expr_binary(left, right, op);
    }

    return left;
}

static struct Expr* parse_additive(struct Parser* p) {
    struct Expr* left = parse_multiplicative(p);

    while (check(p, PLUS) || check(p, MINUS) || check(p, EQUALS_EQUALS) || check(p, NOT_EQUALS) || check(p, GTE)|| check(p, GT)|| check(p, LTE)|| check(p, LT) || check(p, AND) || check(p, OR)) {
        char op;
        if (check(p, EQUALS_EQUALS)) op = 'e';
        else if (check(p, NOT_EQUALS)) op = 'n';
        else if (check(p, GTE)) op = 'g';
        else if (check(p, GT)) op = 'm';
        else if (check(p, LTE)) op = 'l';
        else if (check(p, LT)) op = 'u';
        else if (check(p, AND)) op = 'a';
        else if (check(p, OR)) op = 'o';
        else op = peek(p)->lexeme[0];
        advance(p);
        struct Expr* right = parse_multiplicative(p);
        left = expr_binary(left, right, op);
    }

    return left;
}

static struct Expr* parse_expression(struct Parser* p) {
    struct Expr* left = parse_additive(p);

    while (check(p, PIPE)) {
          advance(p);
          struct Expr* right = parse_primary(p);

          struct Expr* e = malloc(sizeof(struct Expr));
          e->type = EXPR_CALL;

          if (right->type == EXPR_CALL) {
              int new_count = right->as.call.arg_count + 1;
              struct Expr** new_args = malloc(new_count * sizeof(struct Expr*));
              new_args[0] = left;
              for (int i = 0; i < right->as.call.arg_count; i++) {
                  new_args[i + 1] = right->as.call.args[i];
              }
              free(right->as.call.args);
              e->as.call.name = right->as.call.name;
              e->as.call.args = new_args;
              e->as.call.arg_count = new_count;
              free(right);
          } else if (right->type == EXPR_VARIABLE) {
              e->as.call.name = right->as.string;
              e->as.call.args = malloc(sizeof(struct Expr*));
              e->as.call.args[0] = left;
              e->as.call.arg_count = 1;
              free(right);
          } else {
              fprintf(stderr, "Right side of | must be a function\n");
              exit(-1);
          }

          left = e;
      }

      return left;
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

    if (stmt->as.expr->type == EXPR_IF || stmt->as.expr->type == EXPR_WHILE) {
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
