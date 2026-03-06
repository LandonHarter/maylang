#include "may.h"

struct Expr;
void* evaluate(struct Expr* expr);
void* evaluate_urnary(struct Expr* expr);
void* evaluate_binary(struct Expr* expr);
