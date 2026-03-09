#include "types.h"
#include <stdio.h>
#include <string.h>

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

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
    printmv_col(val, COLOR_RESET);
}

void printmv_col(struct MayValue* val, char* color) {
    enum MayValueType type = val->type;
    if (type == MAY_VAR) type = val->inferred;

    char* color_ansi = COLOR_RESET;
    if (strcmp(color, "red") == 0) {
        color_ansi = COLOR_RED;
    } else if (strcmp(color, "green") == 0) {
        color_ansi = COLOR_GREEN;
    } else if (strcmp(color, "yellow") == 0) {
        color_ansi = COLOR_YELLOW;
    } else if (strcmp(color, "blue") == 0) {
        color_ansi = COLOR_BLUE;
    } else if (strcmp(color, "magenta") == 0) {
        color_ansi = COLOR_MAGENTA;
    } else if (strcmp(color, "cyan") == 0) {
        color_ansi = COLOR_CYAN;
    } else if (strcmp(color, "white") == 0) {
        color_ansi = COLOR_RESET;
    }

    switch (type) {
        case MAY_INT:
            printf("%s%i%s\n", color_ansi, val->as.integer, COLOR_RESET);
            break;
        case MAY_FLOAT:
            printf("%s%f%s\n", color_ansi, val->as.floating, COLOR_RESET);
            break;
        case MAY_STRING:
            printf("%s%s%s\n", color_ansi, val->as.string.val, COLOR_RESET);
            break;
        case MAY_FUNC:
            printf("%s%s%s\n", color_ansi, val->as.func.name, COLOR_RESET);
    }
}
