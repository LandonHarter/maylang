#include "types.h"
#include "env.h"
#include "module.h"
#include "lib/html.h"
#include <endian.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* html_string(char* tag, struct MayValue* children, struct MayValue* attributes) {
    size_t total_len = strlen(tag) * 2 + 5;

    if (attributes && attributes->type == MAY_OBJECT) {
        for (int i = 0; i < attributes->as.object.num_values; i++) {
            total_len += 1 + strlen(attributes->as.object.names[i]) + 2
                + strlen(attributes->as.object.values[i]->as.string.val) + 1;
        }
    }

    if (children && children->type == MAY_ARRAY) {
        for (int i = 0; i < children->as.array.length; i++) {
            total_len += strlen(children->as.array.elements[i]->as.string.val);
        }
    }

    char* result = malloc(total_len + 1);
    sprintf(result, "<%s", tag);

    if (attributes && attributes->type == MAY_OBJECT) {
        for (int i = 0; i < attributes->as.object.num_values; i++) {
            strcat(result, " ");
            strcat(result, attributes->as.object.names[i]);
            strcat(result, "=\"");
            strcat(result, attributes->as.object.values[i]->as.string.val);
            strcat(result, "\"");
        }
    }

    strcat(result, ">");

    if (children && children->type == MAY_ARRAY) {
        for (int i = 0; i < children->as.array.length; i++) {
            strcat(result, children->as.array.elements[i]->as.string.val);
        }
    }

    strcat(result, "</");
    strcat(result, tag);
    strcat(result, ">");

    return result;
}

struct MayValue* string_to_value(char* str) {
    struct MayValue* ret = malloc(sizeof(struct MayValue));
    ret->type = MAY_STRING;
    ret->as.string.val = str;
    ret->as.string.len = strlen(str);
    return ret;
}

struct MayValue* html_element(struct MayValue** args, int arg_count) {
    char* tag = args[0]->as.string.val;
    struct MayValue* children = get_field(args[1], "children");
    struct MayValue* attributes = get_field(args[1], "attributes");
    return string_to_value(html_string(tag, children, attributes));
}

struct MayValue* quick_html_element(char* tag, struct MayValue** args) {
    struct MayValue* children = get_field(args[0], "children");
    struct MayValue* attributes = get_field(args[0], "attributes");
    return string_to_value(html_string(tag, children, attributes));
}

struct MayValue* html_html(struct MayValue** args, int arg_count) {
    return quick_html_element("html", args);
}

struct MayValue* html_body(struct MayValue** args, int arg_count) {
    return quick_html_element("body", args);
}

struct MayValue* html_head(struct MayValue** args, int arg_count) {
    return quick_html_element("head", args);
}

struct MayValue* html_p(struct MayValue** args, int arg_count) {
    return quick_html_element("p", args);
}

struct MayValue* html_h1(struct MayValue** args, int arg_count) {
    return quick_html_element("h1", args);
}

struct MayValue* html_h2(struct MayValue** args, int arg_count) {
    return quick_html_element("h2", args);
}

struct MayValue* html_h3(struct MayValue** args, int arg_count) {
    return quick_html_element("h3", args);
}

struct MayValue* html_h4(struct MayValue** args, int arg_count) {
    return quick_html_element("h4", args);
}

struct MayValue* html_h5(struct MayValue** args, int arg_count) {
    return quick_html_element("h5", args);
}

struct MayValue* html_h6(struct MayValue** args, int arg_count) {
    return quick_html_element("h6", args);
}

struct MayValue* html_span(struct MayValue** args, int arg_count) {
    return quick_html_element("span", args);
}

struct MayValue* html_div(struct MayValue** args, int arg_count) {
    return quick_html_element("div", args);
}

struct MayValue* html_img(struct MayValue** args, int arg_count) {
    return quick_html_element("img", args);
}

struct MayValue* html_form(struct MayValue** args, int arg_count) {
    return quick_html_element("form", args);
}

struct MayValue* html_button(struct MayValue** args, int arg_count) {
    return quick_html_element("button", args);
}

struct MayValue* html_video(struct MayValue** args, int arg_count) {
    return quick_html_element("video", args);
}

struct MayValue* html_script(struct MayValue** args, int arg_count) {
    return quick_html_element("script", args);
}

void load_html_env(struct Env* env) {
    enum MayValueType* quick_element = malloc(sizeof(enum MayValueType) * 1);
    quick_element[0] = MAY_OBJECT;

    register_builtin(env, "html", html_html, 1, quick_element);
    register_builtin(env, "body", html_body, 1, quick_element);
    register_builtin(env, "head", html_head, 1, quick_element);
    register_builtin(env, "p", html_p, 1, quick_element);
    register_builtin(env, "h1", html_h1, 1, quick_element);
    register_builtin(env, "h2", html_h2, 1, quick_element);
    register_builtin(env, "h3", html_h3, 1, quick_element);
    register_builtin(env, "h4", html_h4, 1, quick_element);
    register_builtin(env, "h5", html_h5, 1, quick_element);
    register_builtin(env, "h6", html_h6, 1, quick_element);
    register_builtin(env, "span", html_span, 1, quick_element);
    register_builtin(env, "div", html_div, 1, quick_element);
    register_builtin(env, "img", html_img, 1, quick_element);
    register_builtin(env, "form", html_form, 1, quick_element);
    register_builtin(env, "button", html_button, 1, quick_element);
    register_builtin(env, "video", html_video, 1, quick_element);
    register_builtin(env, "script", html_script, 1, quick_element);

    enum MayValueType* element = malloc(sizeof(enum MayValueType) * 1);
    element[0] = MAY_STRING;
    element[1] = MAY_OBJECT;

    register_builtin(env, "element", html_element, 2, element);
}

struct Module* load_html_module() {
    struct Module* mod = malloc(sizeof(struct Module));
    mod->name = "html";
    mod->loader = load_html_env;
    return mod;
}
