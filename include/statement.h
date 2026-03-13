#pragma once
#include <stddef.h>
#include <stdlib.h>

struct Expr;
enum StmtType {
    STMT_EXPR,
    STMT_VARDECL,
    STMT_FUNCDECL,
    STMT_IF,
};

struct Stmt {
    enum StmtType type;

    union {
        struct Expr* expr;
    } as;
};

struct StmtList {
    struct Stmt** stmts;
    size_t count;
    size_t capacity;
};

void stmt_free(struct Stmt* stmt);
void free_stmt_list(struct StmtList* list);
static void append_stmt(struct StmtList* list, struct Stmt* stmt) {
    if (list->count >= list->capacity) {
        list->capacity = list->capacity == 0 ? 8 : list->capacity * 2;
        list->stmts = realloc(list->stmts, list->capacity * sizeof(struct Stmt*));
    }
    list->stmts[list->count++] = stmt;
};
