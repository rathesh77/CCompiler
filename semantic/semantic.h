#include "../ast/ast.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct  {
    ast_list_t *functions;
    ast_list_t *variables;
} ast_symbols;

ast_symbols* create_symbols_table();
bool has_valid_semantic(ast_list_t *);
bool iterate_functions(ast_list_t *);
bool analyze_function(ast_t *, ast_symbols*);

bool analyze_assignment(ast_t *, ast_symbols*);
bool analyze_declaration(ast_t *, ast_symbols*);

bool analyze_fncall(ast_t *, ast_symbols*);