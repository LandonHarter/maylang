#pragma once
#include "may.h"
#include "expression.h"
#include "statement.h"
#include "env.h"

struct TokenList;

struct Parser {
    struct Token* tokens;
    size_t count;
    size_t current;
};

struct StmtList parse(struct TokenList* list, struct Env* env);
