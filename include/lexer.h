#include "token.h"
#include <stddef.h>

struct TokenList {
    struct Token* tokens;
    size_t count;
    size_t capacity;
};

struct TokenList tokenize(char* src);
void free_token_list(struct TokenList* list);
