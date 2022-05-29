#include "./semantic.h"

bool iterate_functions(ast_list_t *tree) {
    ast_list_t *cursor = tree;
    ast_symbols *symbols = malloc(sizeof(ast_symbols));
    ast_list_t *cursor_function = symbols->functions;

    while (cursor->node->type != AST_NULL) {
        if (analyze_function(cursor->node, symbols) == false) {
            return false;
        }
        cursor_function->node = cursor->node;
        cursor_function = cursor_function->next;
        cursor = cursor->next;

    }
    return true;
}
bool analyze_function(ast_t *tree, ast_symbols* symbols) {
    ast_list_t *cursor_stmt = tree->function.stmts;
    ast_list_t *cursor_variable = symbols->variables;

    while (cursor_stmt->node->type != AST_NULL) {
        ast_t *statement = cursor_stmt->node;
        if (statement->type == AST_ASSIGNMENT) {
            bool is_valid_statement = analyze_statement(statement);
            if (!is_valid_statement) {
                return false;
            }
        } else if(statement->type == AST_FNCALL) {
            bool is_valid_fn_call = analyze_fncall(statement);
            if (!is_valid_fn_call) {
                return false;
            }
        }
        cursor_stmt = cursor_stmt->next;
    }
}

bool analyze_statement(ast_t *stmt) {
    return true;
    //return stmt->assignment.lvalue->type == AST_INTEGER && stmt->assignment.rvalue->type == AST_INTEGER;
}

bool analyze_fncall(ast_t *fncall) {
    return false;
}