#include "lexer.h"

int semicolon_ending(struct LexerSource* src) {
    int temp_idx = (int) src->idx - 1;
    if (temp_idx < 0) temp_idx = 0;

    int has_characters = 0;
    int ends_semicolon = 0;
    while (src->src[temp_idx] != '\n') {
        char c = src->src[temp_idx];

        switch (c) {
            case ' ': break;
            case '\t': break;
            case ';':
                if (!has_characters) {
                    ends_semicolon = 1;
                }
                break;
            default:
                has_characters = has_characters || is_alphanumeric(c);
        }

        temp_idx--;
        if (temp_idx < 0) {
            break;
        }
    }

    return !(has_characters && !ends_semicolon);
}
