#pragma once
#include "may.h"
#include "statement.h"

struct Expr;
struct MayValue;
struct Env;
void evaluate(struct StmtList* list, struct Env* env);
void push_import(const char* path);
void pop_import();
struct MayValue* evaluate_block(struct StmtList* list, struct Env* env);
struct MayValue* evaluate_stmt(struct Stmt* stmt, struct Env* env);
struct MayValue* evaluate_expr(struct Expr* expr, struct Env* env);
struct MayValue* evaluate_urnary(struct Expr* expr, struct Env* env);
struct MayValue* evaluate_binary(struct Expr* expr, struct Env* env);
