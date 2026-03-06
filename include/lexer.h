#include "token.h"
#include "types.h"
#include "keyword.h"
#include "rules.h"
#include "source.h"
#include "may.h"

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

struct TokenList tokenize(struct Source* filesource);
void free_token_list(struct TokenList* list);
int follows(struct LexerSource* src, const char* match);

void string_val(struct LexerSource* src, struct MayValue* str);
void number_val(struct LexerSource* src, struct MayValue* num, struct Token* tok);
void identifier_val(struct LexerSource* src, struct Token* tok);
