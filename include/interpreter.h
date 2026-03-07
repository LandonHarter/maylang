#include "may.h"

struct Expr;
struct Stmt;
struct StmtList;
struct MayValue;
struct Env;
void evaluate(struct StmtList* list, struct Env* env);
struct MayValue* evaluate_stmt(struct Stmt* stmt, struct Env* env);
struct MayValue* evaluate_expr(struct Expr* expr, struct Env* env);
struct MayValue* evaluate_urnary(struct Expr* expr, struct Env* env);
struct MayValue* evaluate_binary(struct Expr* expr, struct Env* env);
