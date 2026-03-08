struct MayValue;
struct StmtList;

typedef struct MayValue* (*MayNativeFn)(struct MayValue** args, int arg_count);

enum MayValueType {
    MAY_VAR,
    MAY_INT,
    MAY_FLOAT,
    MAY_STRING,
    MAY_FUNC,
    MAY_BUILTIN_FUNC,
    MAY_RETURN
};

struct MayFunc {
    char* name;
    struct StmtList* body;
    int return_type;
};

struct MayBuiltinFunc {
    char* name;
    int return_type;
    MayNativeFn cfunc;
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
        struct MayFunc func;
        struct MayBuiltinFunc builtin;
    } as;
};

int is_digit(char c);
int is_digit_or_dec(char c);
int is_alpha(char c);
int is_alphanumeric(char c);

void printmv(struct MayValue* val);
