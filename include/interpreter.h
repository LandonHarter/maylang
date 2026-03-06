#include "may.h"

struct Expr;
struct MayValue;
struct MayValue* evaluate(struct Expr* expr);
struct MayValue* evaluate_urnary(struct Expr* expr);
struct MayValue* evaluate_binary(struct Expr* expr);
