#include "may.h"
#include "expression.h"

struct TokenList;

struct Parser {
    struct Token* tokens;
    size_t count;
    size_t current;
};

struct Expr* parse(struct TokenList* list);
