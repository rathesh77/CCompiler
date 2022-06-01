#include "../ast/ast.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct ast_symbols {
    struct ast_list_t *functions;
    struct ast_list_t *variables;
} ast_symbols;


typedef struct ast_list_sym {
    struct ast_symbols *node;
    struct ast_list_sym *previous;
} ast_list_sym;

ast_list_sym* create_symbols_table();
bool has_valid_semantic(ast_list_t *);
bool iterate_functions(ast_list_t *);
bool analyze_function(ast_t *, ast_list_sym*);

bool analyze_assignment(ast_t *, ast_list_sym*);
bool analyze_declaration(ast_t *, ast_list_sym*);

bool analyze_fncall(ast_t *, ast_list_sym*);

void insert_variable(ast_t *, ast_list_t*);
void insert_function(ast_t *, ast_list_t*);