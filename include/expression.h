#pragma once
struct Param {
    char* name;
    int type;
};

enum ExprType {
    EXPR_NUMBER,
    EXPR_STRING,
    EXPR_VARIABLE,
    EXPR_VARIABLEDECL,
    EXPR_FUNC,
    EXPR_FUNCDECL,
    EXPR_FUNCRETURN,
    EXPR_FIELD_ACCESS,
    EXPR_CALL,
    EXPR_IMPORT,
    EXPR_IF,
    EXPR_WHILE,
    EXPR_ASSIGN,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_OBJECT,
    EXPR_ARRAY,
    EXPR_INDEX_ACCESS,
    EXPR_BREAK,
    EXPR_CONTINUE
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
            int exported;
        } vardecl;

        struct {
            char* name;
            struct StmtList* func;
            int decl_type;
            struct Param* params;
            int param_count;
            int exported;
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
            struct Expr* object;
            char* identifier;
        } access;

        struct {
            char* lib;
        } import;

        struct {
            struct Expr* condition;
            struct StmtList* thenbody;
            struct StmtList* elsebody;
        } ifcond;

        struct {
            struct Expr* condition;
            struct StmtList* thenbody;
        } whilecond;

        struct {
            char* name;
            struct Expr* value;
        } assign;

        struct {
            struct Expr* left;
            struct Expr* right;
            char op;
        } binary;

        struct {
            struct Expr* operand;
            char op;
        } unary;

        struct {
            char** keys;
            struct Expr** values;
            int count;
        } object;

        struct {
            struct Expr** elements;
            int count;
        } array;

        struct {
            struct Expr* object;
            struct Expr* index;
        } index_access;
    } as;
};

struct Expr* expr_number(float value);
struct Expr* expr_binary(struct Expr* left, struct Expr* right, char op);
struct Expr* expr_string(char* value);
struct Expr* expr_variable(char* name);
struct Expr* expr_vardecl(char* name, struct Expr* initializer, int decl_type);
struct Expr* expr_funcdecl(char* name, int decl_type, struct StmtList* body, struct Param* params, int param_count);
struct Expr* expr_unary(char op, struct Expr* operand);
void expr_free(struct Expr* expr);
