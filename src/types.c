#include "types.h"

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
