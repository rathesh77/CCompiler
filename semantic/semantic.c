#include "./semantic.h"
#include <string.h>

bool has_valid_semantic(ast_list_t *tree) {
    return iterate_functions(tree);
}

ast_symbols* create_symbols_table() {

    ast_symbols *symbols = malloc(sizeof(ast_symbols));
    symbols->functions = malloc(sizeof(ast_list_t));    
    symbols->variables = malloc(sizeof(ast_list_t));

    symbols->functions->node = malloc(sizeof(ast_t));
    symbols->variables->node = malloc(sizeof(ast_t));

    symbols->functions->node->type = AST_NULL;
    symbols->variables->node->type = AST_NULL;
    return symbols;
}

bool iterate_functions(ast_list_t *tree) {
    ast_list_t *cursor = tree;
    ast_symbols *symbols = create_symbols_table();
    ast_list_t *cursor_function = symbols->functions;
    symbols->functions->node = cursor->node;
    while (cursor->node->type != AST_NULL) {
        if (analyze_function(cursor->node, symbols) == false) {
            return false;
        }
        cursor_function->node = cursor->node;
        cursor_function->next = malloc(sizeof(ast_list_t));
        cursor_function->next->node = malloc(sizeof(ast_t));
        cursor_function->next->node->type = AST_NULL;     
        cursor_function = cursor_function->next;

        cursor = cursor->next;

    }
    return true;
}

bool analyze_function(ast_t *tree, ast_symbols* symbols) {
    ast_list_t *cursor_stmt = NULL;
    if (tree->type == AST_FUNCTION) {
        cursor_stmt = tree->function.stmts;
    } else if (tree->type == AST_CONDITION) {
        cursor_stmt = tree->branch.valid->compound_stmt.stmts;
    } else if(tree->type == AST_LOOP) {
        cursor_stmt = tree->loop.stmts;

    }
    ast_list_t *cursor_variable = symbols->variables;

    while (cursor_stmt->node->type != AST_NULL) {
        ast_t *statement = cursor_stmt->node;
        bool is_valid_statement = false;
        if (statement->type == AST_ASSIGNMENT) {
            is_valid_statement = analyze_assignment(statement, symbols);
        } else if(statement->type == AST_DECLARATION) {
            is_valid_statement = analyze_declaration(statement, symbols);
        } else if(statement->type == AST_FNCALL) {
            is_valid_statement = analyze_fncall(statement, symbols);
        } else if (statement->type == AST_CONDITION || statement->type == AST_LOOP) {
            is_valid_statement = analyze_function(statement, symbols);
        } else if(statement->type == AST_RETURN) {
            is_valid_statement = true;
        }
        if (is_valid_statement == false) {
            return false;
        }
        cursor_stmt = cursor_stmt->next;
    }
    return true;
}

bool analyze_fncall(ast_t *fncall, ast_symbols* symbols) {
    ast_list_t *cursor = symbols->functions;
    while (cursor->node->type != AST_NULL) {
       if (strcmp(cursor->node->function.name, fncall->function.name) == 0) {
            return true;
        }
        cursor->next = malloc(sizeof(ast_list_t));
        cursor->next->node = malloc(sizeof(ast_t));
        cursor->next->node->type = AST_NULL;
        cursor = cursor->next;
    }
    return false;
}

bool analyze_assignment(ast_t * assignment, ast_symbols* symbols) {
    ast_list_t *cursor = symbols->variables;
    while (cursor->node->type != AST_NULL) {
       if (strcmp(cursor->node->declaration.lvalue->var.name, assignment->assignment.lvalue->var.name) == 0 && cursor->node->declaration.lvalue->var.type == assignment->assignment.lvalue->var.type) {
            return true;
        }
        cursor = cursor->next;
    }
    return false;
}

bool analyze_declaration(ast_t * declaration, ast_symbols* symbols) {
    ast_list_t *cursor = symbols->variables;
    while (cursor->node->type != AST_NULL) {
       if (strcmp(cursor->node->var.name, declaration->var.name) == 0 && cursor->node->var.type == declaration->var.type) {
            return false;
        }
        cursor = cursor->next;
    }
    cursor->node = declaration;
    cursor->next = malloc(sizeof(ast_list_t));
    cursor->next->node = malloc(sizeof(ast_t));
    cursor->next->node->type = AST_NULL;
    return true;
}