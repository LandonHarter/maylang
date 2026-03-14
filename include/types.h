#pragma once
struct MayValue;
struct StmtList;

typedef struct MayValue* (*MayNativeFn)(struct MayValue** args, int arg_count);

enum MayValueType {
    MAY_VAR,
    MAY_INT,
    MAY_FLOAT,
    MAY_STRING,
    MAY_OBJECT,
    MAY_ARRAY,
    MAY_FUNC,
    MAY_BUILTIN_FUNC,
    MAY_RETURN,
    MAY_BREAK,
    MAY_CONTINUE
};

struct Param;

struct MayFunc {
    char* name;
    struct StmtList* body;
    int return_type;
    struct Param* params;
    int param_count;
};

struct MayBuiltinFunc {
    char* name;
    int return_type;
    MayNativeFn cfunc;
    int num_args;
    enum MayValueType* arg_types;
};

struct MayValue {
    enum MayValueType type;
    enum MayValueType inferred;
    union {
        void* any;
        int integer;
        float floating;
        struct string {
            char* val;
            int len;
        } string;
        struct object {
            char** names;
            struct MayValue** values;
            int num_values;
        } object;
        struct array_data {
            struct MayValue** elements;
            int length;
        } array;
        struct MayFunc func;
        struct MayBuiltinFunc builtin;
    } as;
};

int is_digit(char c);
int is_digit_or_dec(char c);
int is_alpha(char c);
int is_alphanumeric(char c);

char* mv_to_string(struct MayValue* mv);
void printmv(struct MayValue* val);
void printmv_col(struct MayValue* val, char* color);

char* mvtypestr(enum MayValueType type);

struct MayValue* get_field(struct MayValue* obj, char* name);
