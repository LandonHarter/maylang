enum ExprType {
    EXPR_NUMBER,
    EXPR_STRING,
    EXPR_BINARY,
    EXPR_UNARY
};

struct Expr {
    enum ExprType type;

    union {
        float number;
        char* string;

        struct {
            struct Expr* left;
            struct Expr* right;
            char op;
        } binary;

        struct {
            struct Expr* operand;
            char op;
        } unary;
    } as;
};

struct Expr* expr_number(float value);
struct Expr* expr_binary(struct Expr* left, struct Expr* right, char op);
struct Expr* expr_string(char* value);
struct Expr* expr_unary(char op, struct Expr* operand);
void expr_free(struct Expr* expr);
