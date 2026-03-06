#include "interpreter.h"
#include "expression.h"
#include "types.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>

struct MayValue* evaluate(struct Expr* expr) {
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
        return evaluate_urnary(expr);
    } else if (expr->type == EXPR_BINARY) {
        return evaluate_binary(expr);
    }
    return NULL;
}

struct MayValue* evaluate_urnary(struct Expr* expr) {
    struct MayValue* right = evaluate(expr->as.unary.operand);
    if (right->type != MAY_FLOAT) {
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

struct MayValue* evaluate_binary(struct Expr* expr) {
    struct MayValue* left = evaluate(expr->as.binary.left);
    struct MayValue* right = evaluate(expr->as.binary.right);

    if (left->type != MAY_FLOAT || right->type != MAY_FLOAT) {
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
