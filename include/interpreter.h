#include "may.h"

struct Expr;
struct Stmt;
struct StmtList;
struct MayValue;
void evaluate(struct StmtList* list);
struct MayValue* evaluate_stmt(struct Stmt* stmt);
struct MayValue* evaluate_expr(struct Expr* expr);
struct MayValue* evaluate_urnary(struct Expr* expr);
struct MayValue* evaluate_binary(struct Expr* expr);
