#include "types.h"
#include <stdio.h>
#include <stdlib.h>
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

char* mv_to_string(struct MayValue* mv) {
    if (mv == NULL) return NULL;

    enum MayValueType type = mv->type;
    if (type == MAY_VAR) type = mv->inferred;

    switch (type) {
        case MAY_INT:
            char* intstr = malloc(sizeof(char) * 16);
            snprintf(intstr, 16, "%i", mv->as.integer);
            return intstr;
        case MAY_FLOAT:
            char* floatstr = malloc(sizeof(char) * 16);
            snprintf(floatstr, 16, "%g", mv->as.floating);
            return floatstr;
        case MAY_STRING:
            return mv->as.string.val;
    }
    return NULL;
}

void printmv(struct MayValue* val) {
    if (!val) return;
    printmv_col(val, COLOR_RESET);
}

static char* resolve_color(char* color) {
    if (strcmp(color, "red") == 0) return COLOR_RED;
    if (strcmp(color, "green") == 0) return COLOR_GREEN;
    if (strcmp(color, "yellow") == 0) return COLOR_YELLOW;
    if (strcmp(color, "blue") == 0) return COLOR_BLUE;
    if (strcmp(color, "magenta") == 0) return COLOR_MAGENTA;
    if (strcmp(color, "cyan") == 0) return COLOR_CYAN;
    return COLOR_RESET;
}

static void print_indent(char* color_ansi, int depth) {
    printf("%s", color_ansi);
    for (int i = 0; i < depth; i++) printf("  ");
}

static void printmv_inner(struct MayValue* val, char* color, char* color_ansi, int depth) {
    enum MayValueType type = val->type;
    if (type == MAY_VAR) type = val->inferred;

    if (type == MAY_ARRAY) {
        printf("%s[%s\n", color_ansi, COLOR_RESET);
        for (int i = 0; i < val->as.array.length; i++) {
            print_indent(color_ansi, depth + 1);
            printmv_inner(val->as.array.elements[i], color, color_ansi, depth + 1);
            if (i < val->as.array.length - 1)
                printf("%s,", color_ansi);
            printf("\n");
        }
        print_indent(color_ansi, depth);
        printf("]%s", COLOR_RESET);
        return;
    }

    if (type == MAY_OBJECT) {
        printf("%s{%s\n", color_ansi, COLOR_RESET);
        for (int i = 0; i < val->as.object.num_values; i++) {
            print_indent(color_ansi, depth + 1);
            printf("%s: ", val->as.object.names[i]);
            printmv_inner(val->as.object.values[i], color, color_ansi, depth + 1);
            if (i < val->as.object.num_values - 1)
                printf("%s,", color_ansi);
            printf("\n");
        }
        print_indent(color_ansi, depth);
        printf("}%s", COLOR_RESET);
        return;
    }

    printf("%s%s%s", color_ansi, mv_to_string(val), COLOR_RESET);
}

void printmv_col(struct MayValue* val, char* color) {
    char* color_ansi = resolve_color(color);
    printmv_inner(val, color, color_ansi, 0);
    printf("\n");
}

char* mvtypestr(enum MayValueType type) {
    switch (type) {
        case MAY_INT:
            return "int";
        case MAY_FLOAT:
            return "float";
        case MAY_STRING:
            return "string";
        case MAY_ARRAY:
            return "array";
        default:
            return NULL;
    }
}

struct MayValue* get_field(struct MayValue* obj, char* name) {
    for (int i = 0; i < obj->as.object.num_values; i++) {
        if (strcmp(obj->as.object.names[i], name) == 0) {
            return obj->as.object.values[i];
        }
    } 
    return NULL;
}
