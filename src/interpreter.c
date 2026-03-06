#include "interpreter.h"
#include "expression.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* evaluate(struct Expr* expr) {
    if (expr->type == EXPR_NUMBER) {
        return &expr->as.number;
    } else if (expr->type == EXPR_UNARY) {
        return evaluate_urnary(expr);
    } else if (expr->type == EXPR_BINARY) {
        return evaluate_binary(expr);
    }
    return NULL;
}

void* evaluate_urnary(struct Expr* expr) {
    void* right = evaluate(expr->as.unary.operand);
    float rightf = *((float*)right);

    float* result = malloc(sizeof(float));
    switch (expr->as.unary.op) {
        case '-':
            *result = -rightf;
            break;
        case '!':
            *result = rightf > 0 ? 0 : 1;
            break;
        default:
            free(result);
            return NULL;
    }
    return result;
}

void* evaluate_binary(struct Expr* expr) {
    void* left = evaluate(expr->as.binary.left);
    void* right = evaluate(expr->as.binary.right);

    float leftf = *((float*)left);
    float rightf = *((float*)right);

    float* result = malloc(sizeof(float));
    switch (expr->as.binary.op) {
        case '+': *result = leftf + rightf; break;
        case '-': *result = leftf - rightf; break;
        case '*': *result = leftf * rightf; break;
        case '/': *result = leftf / rightf; break;
        default: free(result); return NULL;
    }
    return result;
}
