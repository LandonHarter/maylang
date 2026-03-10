#include "interpreter.h"
#include "expression.h"
#include "token.h"
#include "types.h"
#include "error.h"
#include "statement.h"
#include "env.h"
#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static int is_numeric(struct MayValue* val) {
    enum MayValueType t = val->type == MAY_VAR ? val->inferred : val->type;
    return t == MAY_FLOAT || t == MAY_INT;
}

static struct MayValue* lookup_var(struct Env* env, const char* name) {
    for (struct Env* e = env; e != NULL; e = e->parent) {
        for (size_t i = 0; i < e->vars->count; i++) {
            if (strcmp(e->vars->vars[i]->name, name) == 0) {
                return e->vars->vars[i]->value;
            }
        }
    }
    return NULL;
}

struct MayValue* evaluate_expr(struct Expr* expr, struct Env* env) {
    if (expr->type == EXPR_NUMBER) {
        struct MayValue* result = malloc(sizeof(struct MayValue));
        result->type = MAY_FLOAT;
        result->as.floating = expr->as.number;
        return result;
    } else if (expr->type == EXPR_STRING) {
        struct MayValue* result = malloc(sizeof(struct MayValue));
        result->type = MAY_STRING;
        result->as.string.val = expr->as.string;
        result->as.string.len = strlen(expr->as.string);
        return result;
    } else if (expr->type == EXPR_UNARY) {
        return evaluate_urnary(expr, env);
    } else if (expr->type == EXPR_BINARY) {
        return evaluate_binary(expr, env);
    } else if (expr->type == EXPR_VARIABLEDECL) {
        struct MayValue* value = NULL;
        if (expr->as.vardecl.initializer)
            value = evaluate_expr(expr->as.vardecl.initializer, env);

        int decl = expr->as.vardecl.decl_type;
        if (value) {
            if (decl == VAR) {
                value->inferred = value->type;
                value->type = MAY_VAR;
                debug_printf("It is recommended to use a definite type instead of var");
            } else if (decl == INT) {
                if (value->type == MAY_FLOAT) {
                    value->as.integer = (int)value->as.floating;
                } else if (value->type != MAY_INT) {
                    throw_runtime_error("Cannot assign non-integer to int variable");
                }
                value->type = MAY_INT;
                value->inferred = MAY_INT;
            } else if (decl == FLOAT) {
                if (value->type == MAY_INT) {
                    value->as.floating = (float)value->as.integer;
                } else if (value->type != MAY_FLOAT) {
                    throw_runtime_error("Cannot assign non-float to float variable");
                }
                value->type = MAY_FLOAT;
                value->inferred = MAY_FLOAT;
            } else if (decl == STRING_TYPE) {
                if (value->type != MAY_STRING) {
                    throw_runtime_error("Cannot assign non-string to string variable");
                }
                value->type = MAY_STRING;
                value->inferred = MAY_STRING;
            } else if (decl == OBJECT_TYPE) {
                if (value->type != MAY_OBJECT) {
                    throw_runtime_error("Cannot assign non-object to object variable");
                }
                value->inferred = MAY_OBJECT;
            } else if (decl == ARRAY_TYPE) {
                if (value->type != MAY_ARRAY) {
                    throw_runtime_error("Cannot assign non-array to array variable");
                }
                value->inferred = MAY_ARRAY;
            }
        }

        struct Var* existing = NULL;
        for (struct Env* e = env; e != NULL; e = e->parent) {
            for (size_t i = 0; i < e->vars->count; i++) {
                if (strcmp(e->vars->vars[i]->name, expr->as.vardecl.name) == 0) {
                    existing = e->vars->vars[i];
                    break;
                }
            }
            if (existing) break;
        }

        if (existing) {
            existing->value = value;
        } else {
            struct Var* var = malloc(sizeof(struct Var));
            var->name = strdup(expr->as.vardecl.name);
            var->value = value;
            append_var(env, var);
        }
        return value;
    } else if (expr->type == EXPR_FUNCDECL) {
        struct MayValue* value = malloc(sizeof(struct MayValue));
        value->type = MAY_FUNC;
        value->as.func.name = strdup(expr->as.funcdecl.name);
        value->as.func.body = expr->as.funcdecl.func;
        value->as.func.return_type = expr->as.funcdecl.decl_type;
        value->as.func.params = expr->as.funcdecl.params;
        value->as.func.param_count = expr->as.funcdecl.param_count;

        struct Var* var = malloc(sizeof(struct Var));
        var->name = strdup(expr->as.funcdecl.name);
        var->value = value;
        append_var(env, var);
        return value;
    } else if (expr->type == EXPR_FUNCRETURN) {
        struct MayValue* val = evaluate_expr(expr->as.funcreturn.expr, env);
        struct MayValue* ret = malloc(sizeof(struct MayValue));
        *ret = *val;
        ret->inferred = val->type;
        ret->type = MAY_RETURN;
        return ret;
    } else if (expr->type == EXPR_CALL) {
        struct MayValue* func = lookup_var(env, expr->as.call.name);
        if (!func) {
            throw_runtime_error("Undefined function");
        }
        if (func->type != MAY_FUNC && func->type != MAY_BUILTIN_FUNC) {
            throw_runtime_error("Not a function");
        }

        if (func->type == MAY_BUILTIN_FUNC) {
            struct MayValue** args = NULL;
            int argc = expr->as.call.arg_count;
            if (func->as.builtin.num_args != argc) {
                throw_runtime_error("Wrong number of arguments");
            }

            if (argc > 0) {
                args = malloc(argc * sizeof(struct MayValue*));
                for (int i = 0; i < argc; i++) {
                    args[i] = evaluate_expr(expr->as.call.args[i], env);
                    if (args[i]->type != func->as.builtin.arg_types[i] && func->as.builtin.arg_types[i] != MAY_VAR) {
                        throw_runtime_error("Invalid argument type");
                    }
                }
            }
            struct MayValue* ret = func->as.builtin.cfunc(args, argc);
            free(args);
            return ret;
        }

        if (expr->as.call.arg_count != func->as.func.param_count) {
            throw_runtime_error("Wrong number of arguments");
        }

        struct MayValue* result = NULL;
        struct Env* call_env = new_env(env);

        for (int i = 0; i < func->as.func.param_count; i++) {
            struct MayValue* arg_val = evaluate_expr(expr->as.call.args[i], env);
            struct MayValue* param_val = malloc(sizeof(struct MayValue));
            *param_val = *arg_val;

            int ptype = func->as.func.params[i].type;
            if (ptype == INT) {
                if (param_val->type == MAY_FLOAT) {
                    param_val->as.integer = (int)param_val->as.floating;
                }
                param_val->type = MAY_INT;
                param_val->inferred = MAY_INT;
            } else if (ptype == FLOAT) {
                if (param_val->type == MAY_INT) {
                    param_val->as.floating = (float)param_val->as.integer;
                }
                param_val->type = MAY_FLOAT;
                param_val->inferred = MAY_FLOAT;
            } else if (ptype == STRING_TYPE) {
                if (param_val->type != MAY_STRING) {
                    throw_runtime_error("Cannot pass non-string to string parameter");
                }
                param_val->type = MAY_STRING;
                param_val->inferred = MAY_STRING;
            } else {
                param_val->inferred = param_val->type;
                param_val->type = MAY_VAR;
            }

            struct Var* var = malloc(sizeof(struct Var));
            var->name = strdup(func->as.func.params[i].name);
            var->value = param_val;
            append_var(call_env, var);
        }

        for (int i = 0; i < func->as.func.body->count; i++) {
            result = evaluate_stmt(func->as.func.body->stmts[i], call_env);
            if (result && result->type == MAY_RETURN) {
                result->type = result->inferred;
                break;
            }
        }
        free_env(call_env);
        return result;
    } else if (expr->type == EXPR_ASSIGN) {
        struct MayValue* value = evaluate_expr(expr->as.assign.value, env);
        struct MayValue* existing = lookup_var(env, expr->as.assign.name);
        if (!existing) {
            throw_runtime_error("Undefined variable");
        }
        *existing = *value;
        return existing;
    } else if (expr->type == EXPR_VARIABLE) {
        struct MayValue* value = lookup_var(env, expr->as.string);
        if (!value) {
            throw_runtime_error("Undefined variable");
        }
        return value;
    } else if (expr->type == EXPR_IMPORT) {
        load_module(env, expr->as.import.lib);
    } else if (expr->type == EXPR_IF) {
        struct Expr* cond_expr = expr->as.ifcond.condition;
        struct MayValue* cond = evaluate_expr(cond_expr, env);
        int truthy = (cond->type == MAY_FLOAT && cond->as.floating == 1)
                  || (cond->type == MAY_INT && cond->as.integer != 0);
        if (truthy) {
            struct Env* ifenv = new_env(env);
            struct MayValue* ret = evaluate_block(expr->as.ifcond.thenbody, ifenv);
            if (ret && ret->type == MAY_RETURN) return ret;
        }
    } else if (expr->type == EXPR_WHILE) {
        struct Expr* cond_expr = expr->as.whilecond.condition;
        struct MayValue* cond = evaluate_expr(cond_expr, env);
        while ((cond->type == MAY_FLOAT && cond->as.floating == 1)
            || (cond->type == MAY_INT && cond->as.integer != 0)) {
            struct Env* whileenv = new_env(env);
            struct MayValue* ret = evaluate_block(expr->as.whilecond.thenbody, whileenv);
            if (ret && ret->type == MAY_RETURN) return ret;
            cond = evaluate_expr(cond_expr, env);
        }
    } else if (expr->type == EXPR_FIELD_ACCESS) {
        struct MayValue* obj = evaluate_expr(expr->as.access.object, env);
        if (obj->type != MAY_OBJECT) {
            printf("%i\n", obj->type);
            throw_runtime_error("Cannot access field on non object type");
        }

        for (int i = 0; i < obj->as.object.num_values; i++) {
            if (strcmp(obj->as.object.names[i], expr->as.access.identifier) == 0) {
                return obj->as.object.values[i];
            }
        }
    } else if (expr->type == EXPR_ARRAY) {
        struct MayValue* result = malloc(sizeof(struct MayValue));
        result->type = MAY_ARRAY;
        result->as.array.length = expr->as.array.count;
        result->as.array.elements = malloc(expr->as.array.count * sizeof(struct MayValue*));
        for (int i = 0; i < expr->as.array.count; i++) {
            result->as.array.elements[i] = evaluate_expr(expr->as.array.elements[i], env);
        }
        return result;
    } else if (expr->type == EXPR_INDEX_ACCESS) {
        struct MayValue* obj = evaluate_expr(expr->as.index_access.object, env);
        struct MayValue* idx = evaluate_expr(expr->as.index_access.index, env);
        int index;
        if (idx->type == MAY_INT) index = idx->as.integer;
        else if (idx->type == MAY_FLOAT) index = (int)idx->as.floating;
        else {
            throw_runtime_error("Index must be a number");
            return NULL;
        }
        if (obj->type == MAY_ARRAY) {
            if (index < 0 || index >= obj->as.array.length) {
                throw_runtime_error("Array index out of bounds");
                return NULL;
            }
            return obj->as.array.elements[index];
        } else if (obj->type == MAY_OBJECT) {
            if (index < 0 || index >= obj->as.object.num_values) {
                throw_runtime_error("Object index out of bounds");
                return NULL;
            }
            return obj->as.object.values[index];
        }
        throw_runtime_error("Cannot index non-array type");
        return NULL;
    } else if (expr->type == EXPR_OBJECT) {
        struct MayValue* result = malloc(sizeof(struct MayValue));
        result->type = MAY_OBJECT;
        result->as.object.num_values = expr->as.object.count;
        result->as.object.names = malloc(expr->as.object.count * sizeof(char*));
        result->as.object.values = malloc(expr->as.object.count * sizeof(struct MayValue*));
        for (int i = 0; i < expr->as.object.count; i++) {
            result->as.object.names[i] = strdup(expr->as.object.keys[i]);
            result->as.object.values[i] = evaluate_expr(expr->as.object.values[i], env);
        }
        return result;
    }

    return NULL;
}

struct MayValue* evaluate_stmt(struct Stmt* stmt, struct Env* env) {
    if (stmt->type == STMT_EXPR || stmt->type == STMT_VARDECL || stmt->type == STMT_FUNCDECL) {
        return evaluate_expr(stmt->as.expr, env);
    }
    return NULL;
}

void evaluate(struct StmtList* list, struct Env* env) {
    for (int i = 0; i < list->count; i++) {
        struct MayValue* ret = evaluate_stmt(list->stmts[i], env);
    }
}

struct MayValue* evaluate_block(struct StmtList* list, struct Env* env) {
    for (int i = 0; i < list->count; i++) {
        struct MayValue* ret = evaluate_stmt(list->stmts[i], env);
        if (ret && ret->type == MAY_RETURN) return ret;
    }
    return NULL;
}

struct MayValue* evaluate_urnary(struct Expr* expr, struct Env* env) {
    struct MayValue* right = evaluate_expr(expr->as.unary.operand, env);
    if (!is_numeric(right)) {
        throw_runtime_error("Operand must be number");
    }
    float rightf = right->type == MAY_INT ? (float)right->as.integer : right->as.floating;

    struct MayValue* result = malloc(sizeof(struct MayValue));
    result->type = MAY_FLOAT;
    switch (expr->as.unary.op) {
        case '-':
            result->as.floating = -rightf;
            break;
        case '!':
            result->as.floating = rightf > 0 ? 0 : 1;
            break;
        default:
            free(result);
            return NULL;
    }
    return result;
}

struct MayValue* evaluate_binary(struct Expr* expr, struct Env* env) {
    struct MayValue* left = evaluate_expr(expr->as.binary.left, env);
    struct MayValue* right = evaluate_expr(expr->as.binary.right, env);
    struct MayValue* result = malloc(sizeof(struct MayValue));

    if (is_numeric(left) && is_numeric(right)) {
        float leftf = left->type == MAY_INT ? (float)left->as.integer : left->as.floating;
        float rightf = right->type == MAY_INT ? (float)right->as.integer : right->as.floating;

        result->type = MAY_FLOAT;
        switch (expr->as.binary.op) {
            case '+': result->as.floating = leftf + rightf; break;
            case '-': result->as.floating = leftf - rightf; break;
            case '*': result->as.floating = leftf * rightf; break;
            case '/': result->as.floating = leftf / rightf; break;
            case '%': result->as.floating = (int)floorf(leftf) % (int)floorf(rightf); break;
            case 'e': result->as.floating = leftf == rightf ? 1 : 0; break;
            case 'n': result->as.floating = leftf != rightf ? 1 : 0; break;
            case 'g': result->as.floating = leftf >= rightf ? 1 : 0; break;
            case 'm': result->as.floating = leftf > rightf ? 1 : 0; break;
            case 'l': result->as.floating = leftf <= rightf ? 1 : 0; break;
            case 'u': result->as.floating = leftf < rightf ? 1 : 0; break;
            case 'a': result->as.floating = (leftf == 1 && rightf == 1) ? 1 : 0; break;
            case 'o': result->as.floating = (leftf == 1 || rightf == 1) ? 1 : 0; break;
            default: free(result); return NULL;
        }
        return result;
    } else if (left->type == MAY_STRING || right->type == MAY_STRING) {
        if (expr->as.binary.op == '+') {
            result->type = MAY_STRING;
            int left_len = strlen(mv_to_string(left));
            int right_len = strlen(mv_to_string(right));
            char* resstr = malloc(left_len + right_len + 1);
            memcpy(resstr, mv_to_string(left), left_len);
            memcpy(resstr + left_len, mv_to_string(right), right_len);
            resstr[left_len + right_len] = '\0';
            result->as.string.val = resstr;
            result->as.string.len = left_len + right_len;
            return result;
        } else if (expr->as.binary.op == 'e') {
            result->type = MAY_FLOAT;
            result->as.floating = strcmp(left->as.string.val, right->as.string.val) == 0 ? 1 : 0;
            return result;
        } else if (expr->as.binary.op == 'n') {
            result->type = MAY_FLOAT;
            result->as.floating = strcmp(left->as.string.val, right->as.string.val) != 0 ? 1 : 0;
            return result;
        }
    }

    fprintf(stderr, "Cannot %c types %s and %s\n", expr->as.binary.op, mvtypestr(left->type), mvtypestr(right->type));
    exit(-1);
    return NULL;
}
