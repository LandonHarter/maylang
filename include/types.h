enum MayValueType {
    MAY_VAR,
    MAY_INT,
    MAY_FLOAT,
    MAY_STRING
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
    } as;
};

int is_digit(char c);
int is_digit_or_dec(char c);
int is_alpha(char c);
int is_alphanumeric(char c);

void printmv(struct MayValue* val);
