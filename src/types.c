#include "types.h"
#include <stdio.h>

int is_digit(char c) {
    return c >= '0' && c <= '9';
}

int is_digit_or_dec(char c) {
    return is_digit(c) || c == '.';
}

int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

int is_alphanumeric(char c) {
    return is_alpha(c) || is_digit(c);
}

void printmv(struct MayValue* val) {
    if (!val) return;

    enum MayValueType type = val->type;
    if (type == MAY_VAR) type = val->inferred;
    switch (type) {
        case MAY_INT:
            printf("%i\n", val->as.integer);
            break;
        case MAY_FLOAT:
            printf("%f\n", val->as.floating);
            break;
        case MAY_STRING:
            printf("%s\n", val->as.string.val);
            break;
    }
}
