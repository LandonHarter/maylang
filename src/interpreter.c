#include "interpreter.h"
#include "expression.h"
#include "token.h"
#include "types.h"
#include "error.h"
#include "statement.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void append_var(struct Env* env, struct Var* var) {
    if (env->vars->count >= env->vars->capacity) {
        env->vars->capacity = env->vars->capacity == 0 ? 8 : env->vars->capacity * 2;
        env->vars->vars = realloc(env->vars->vars, env->vars->capacity * sizeof(struct Var));
    }
    env->vars->vars[env->vars->count++] = var;
}

static int is_numeric(struct MayValue* val) {
    enum MayValueType t = val->type == MAY_VAR ? val->inferred : val->type;
    return t == MAY_FLOAT || t == MAY_INT;
}

static struct MayValue* lookup_var(struct Env* env, const char* name) {
    for (size_t i = 0; i < env->vars->count; i++) {
        if (strcmp(env->vars->vars[i]->name, name) == 0) {
            return env->vars->vars[i]->value;
        }
    }
    return NULL;
}

struct MayValue* evaluate_expr(struct Expr* expr, struct Env* env) {
    if (expr->type == EXPR_NUMBER) {
        struct MayValue* result = malloc(sizeof(struct MayValue));
        result->type = MAY_FLOAT;
        result->as.floating = expr->as.number;
        return result;
    } else if (expr->type == EXPR_STRING) {
        struct MayValue* result = malloc(sizeof(struct MayValue));
        result->type = MAY_STRING;
        result->as.string.val = expr->as.string;
        result->as.string.len = strlen(expr->as.string);
        return result;
    } else if (expr->type == EXPR_UNARY) {
        return evaluate_urnary(expr, env);
    } else if (expr->type == EXPR_BINARY) {
        return evaluate_binary(expr, env);
    } else if (expr->type == EXPR_VARIABLEDECL) {
        struct MayValue* value = NULL;
        if (expr->as.vardecl.initializer)
            value = evaluate_expr(expr->as.vardecl.initializer, env);

        int decl = expr->as.vardecl.decl_type;
        if (value) {
            if (decl == VAR) {
                value->inferred = value->type;
                value->type = MAY_VAR;
            } else if (decl == INT) {
                if (value->type == MAY_FLOAT) {
                    value->as.integer = (int)value->as.floating;
                } else if (value->type != MAY_INT) {
                    throw_runtime_error("Cannot assign non-integer to int variable");
                }
                value->type = MAY_INT;
                value->inferred = MAY_INT;
            } else if (decl == FLOAT) {
                if (value->type == MAY_INT) {
                    value->as.floating = (float)value->as.integer;
                } else if (value->type != MAY_FLOAT) {
                    throw_runtime_error("Cannot assign non-float to float variable");
                }
                value->type = MAY_FLOAT;
                value->inferred = MAY_FLOAT;
            }
        }

        struct Var* var = malloc(sizeof(struct Var));
        var->name = strdup(expr->as.vardecl.name);
        var->value = value;
        append_var(env, var);
        return value;
    } else if (expr->type == EXPR_VARIABLE) {
        struct MayValue* value = lookup_var(env, expr->as.string);
        if (!value) {
            throw_runtime_error("Undefined variable");
        }
        return value;
    }

    return NULL;
}

struct MayValue* evaluate_stmt(struct Stmt* stmt, struct Env* env) {
    if (stmt->type == STMT_EXPR || stmt->type == STMT_VARDECL) {
        return evaluate_expr(stmt->as.expr, env);
    }
    return NULL;
}

void evaluate(struct StmtList* list, struct Env* env) {
    for (int i = 0; i < list->count; i++) {
        struct MayValue* ret = evaluate_stmt(list->stmts[i], env);
    }
}

struct MayValue* evaluate_urnary(struct Expr* expr, struct Env* env) {
    struct MayValue* right = evaluate_expr(expr->as.unary.operand, env);
    if (!is_numeric(right)) {
        throw_runtime_error("Operand must be number");
    }
    float rightf = right->as.floating;

    struct MayValue* result = malloc(sizeof(struct MayValue));
    result->type = MAY_FLOAT;
    switch (expr->as.unary.op) {
        case '-':
            result->as.floating = -rightf;
            break;
        case '!':
            result->as.floating = rightf > 0 ? 0 : 1;
            break;
        default:
            free(result);
            return NULL;
    }
    return result;
}

struct MayValue* evaluate_binary(struct Expr* expr, struct Env* env) {
    struct MayValue* left = evaluate_expr(expr->as.binary.left, env);
    struct MayValue* right = evaluate_expr(expr->as.binary.right, env);

    if (!is_numeric(left) || !is_numeric(right)) {
        throw_runtime_error("Operands must be numbers");
    }

    float leftf = left->as.floating;
    float rightf = right->as.floating;

    struct MayValue* result = malloc(sizeof(struct MayValue));
    result->type = MAY_FLOAT;
    switch (expr->as.binary.op) {
        case '+': result->as.floating = leftf + rightf; break;
        case '-': result->as.floating = leftf - rightf; break;
        case '*': result->as.floating = leftf * rightf; break;
        case '/': result->as.floating = leftf / rightf; break;
        default: free(result); return NULL;
    }
    return result;
}
