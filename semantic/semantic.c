#include "./semantic.h"
#include <string.h>

bool has_valid_semantic(ast_list_t *tree) {
    return iterate_functions(tree);
}

ast_list_sym* create_symbols_table() {

    ast_list_sym *table = malloc(sizeof(ast_list_sym));
    ast_symbols *symbols = malloc(sizeof(ast_symbols));
    symbols->functions = malloc(sizeof(ast_list_t));    
    symbols->variables = malloc(sizeof(ast_list_t));

    symbols->functions->node = malloc(sizeof(ast_t));
    symbols->variables->node = malloc(sizeof(ast_t));

    symbols->functions->node->type = AST_NULL;
    symbols->variables->node->type = AST_NULL;

    symbols->functions->next = malloc(sizeof(ast_t));
    symbols->variables->next = malloc(sizeof(ast_t));


    table->node = symbols;
    table->previous = NULL;
    return table;
}

bool iterate_functions(ast_list_t *tree) {
    ast_list_t *cursor = tree;
    ast_list_sym *list = create_symbols_table();
    
    while (cursor->node->type != AST_NULL) {
        insert_function(cursor->node, list->node->functions);
        ast_list_sym *new_list = create_symbols_table();
        new_list->node->functions = list->node->functions;
        if (analyze_function(cursor->node, new_list) == false) {
            return false;
        }

        cursor = cursor->next;

    }
    return true;
}

bool analyze_function(ast_t *tree, ast_list_sym* list) {
    ast_list_t *cursor_stmt = NULL;
    ast_symbols *cursor_sym = list->node;
    if (tree->type == AST_FUNCTION) {
        cursor_stmt = tree->function.stmts;
    } else if (tree->type == AST_CONDITION) {
        cursor_stmt = tree->branch.valid->compound_stmt.stmts;
    } else if(tree->type == AST_LOOP) {
        cursor_stmt = tree->loop.stmts;

    }
    ast_list_t *cursor_variable = cursor_sym->variables;

    while (cursor_stmt->node->type != AST_NULL) {
        ast_t *statement = cursor_stmt->node;
        bool is_valid_statement = false;
        if (statement->type == AST_ASSIGNMENT) {
            is_valid_statement = analyze_assignment(statement, list);
        } else if(statement->type == AST_DECLARATION) {
            is_valid_statement = analyze_declaration(statement, list);
        } else if(statement->type == AST_FNCALL) {            
            is_valid_statement = analyze_fncall(statement, list);
        } else if (statement->type == AST_CONDITION || statement->type == AST_LOOP) {
            ast_list_sym *new_list = create_symbols_table();
            new_list->previous = list;            
            is_valid_statement = analyze_function(statement, new_list);
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

bool analyze_fncall(ast_t *fncall, ast_list_sym* list) {
    ast_list_sym *cursor_list = list;

    while (cursor_list != NULL) {
        ast_list_t *functions = cursor_list->node->functions;
        while (functions->node->type != AST_NULL) {
            ast_t *current_function = functions->node;
            if (strcmp(current_function->function.name, fncall->function.name) == 0) {
                ast_list_t *func_cursor_params = current_function->function.params;
                ast_list_t *call_cursor_params = fncall->call.args;

                while (func_cursor_params != NULL) {
                    ast_t *current_call_param = call_cursor_params->node;
                    ast_t *current_fn_param = func_cursor_params->node;

                    if (current_fn_param->type != current_call_param->type)
                        if (current_fn_param->type == AST_INTEGER && current_call_param->type == AST_VARIABLE && call_cursor_params->node->var.type != AST_INTEGER)
                            return false;
                    
                    func_cursor_params = func_cursor_params->next;
                    call_cursor_params = call_cursor_params->next;
                    if (call_cursor_params == NULL && func_cursor_params != NULL)
                        return false;
                 }
                return true;
            }
            functions = functions->next;

        }
        cursor_list = cursor_list->previous;
    }    

    printf("function \"%s\" non défini\n", fncall->call.name);

    return false;
}

bool analyze_assignment(ast_t * assignment, ast_list_sym* list) {
    ast_list_sym *cursor_list = list;

    while (cursor_list != NULL) {
        ast_list_t *variables = cursor_list->node->variables;
        while (variables->node->type != AST_NULL) {
            ast_t *current_variable = variables->node;
            if (strcmp(current_variable->declaration.lvalue->var.name, assignment->assignment.lvalue->var.name) == 0 && current_variable->declaration.lvalue->var.type == assignment->assignment.lvalue->var.type) {
                return true;
            }
            variables = variables->next;
        }
        cursor_list = cursor_list->previous;
    }
    printf("variable \"%s\" non declaré\n", assignment->assignment.lvalue->var.name);
    return false;
}

bool analyze_declaration(ast_t * declaration, ast_list_sym* list) {
    ast_list_t *variables  = list->node->variables;
    while (variables->node->type != AST_NULL) {
        ast_t *current_variable = variables->node;
        if (strcmp(current_variable->declaration.lvalue->var.name, declaration->declaration.lvalue->var.name) == 0 && current_variable->declaration.lvalue->var.type ==declaration->declaration.lvalue->var.type) {
                return false;
        }
        variables = variables->next;
    }    
    insert_variable(declaration, list->node->variables);

    return true;
}

void insert_variable(ast_t *var, ast_list_t*vars) {
    ast_list_t* cursor = vars;

    while (cursor->node->type != AST_NULL) {

        cursor = cursor->next;
    } 
    cursor->node = var;
    cursor->next = malloc(sizeof(ast_t));
    cursor->next->node = malloc(sizeof(ast_t));

    cursor->next->node->type = AST_NULL;
}

void insert_function(ast_t *fun, ast_list_t*funcs) {
    ast_list_t* cursor = funcs;

    while (cursor->node->type != AST_NULL) {

        cursor = cursor->next;
    } 
    cursor->node = fun;
    cursor->next = malloc(sizeof(ast_t));
    cursor->next->node = malloc(sizeof(ast_t));

    cursor->next->node->type = AST_NULL;
}