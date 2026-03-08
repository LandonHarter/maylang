enum ExprType {
    EXPR_NUMBER,
    EXPR_STRING,
    EXPR_VARIABLE,
    EXPR_VARIABLEDECL,
    EXPR_FUNC,
    EXPR_FUNCDECL,
    EXPR_FUNCRETURN,
    EXPR_CALL,
    EXPR_BINARY,
    EXPR_UNARY
};

struct Expr {
    enum ExprType type;

    union {
        float number;
        char* string;

        struct {
            char* name;
            struct Expr* initializer;
            int decl_type;
        } vardecl;

        struct {
            char* name;
            struct StmtList* func;
            int decl_type;
        } funcdecl;

        struct {
            struct Expr* expr;
        } funcreturn;

        struct {
            char* name;
            struct Expr** args;
            int arg_count;
        } call;

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
struct Expr* expr_variable(char* name);
struct Expr* expr_vardecl(char* name, struct Expr* initializer, int decl_type);
struct Expr* expr_funcdecl(char* name, int decl_type, struct StmtList* body);
struct Expr* expr_unary(char op, struct Expr* operand);
void expr_free(struct Expr* expr);
