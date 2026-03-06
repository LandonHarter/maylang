#include "statement.h"

void free_stmt_list(struct StmtList* list) {
    free(list->stmts);
    list->stmts = NULL;
    list->capacity = 0;
    list->count = 0;
}
