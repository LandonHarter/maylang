#include "token.h"
#include "types.h"
#include <stddef.h>

struct LexerSource {
    char* src;

    unsigned int idx;
    unsigned int line;
};

struct TokenList {
    struct Token* tokens;
    size_t count;
    size_t capacity;
};

struct TokenList tokenize(char* src);
void free_token_list(struct TokenList* list);

void string_val(struct LexerSource* src, struct GOMString* str);
void number_val(struct LexerSource* src, float* num, struct Token* tok);
