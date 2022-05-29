#include "../ast/ast.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct  {
    ast_list_t *functions;
    ast_list_t *variables;
} ast_symbols;

bool iterate_functions(ast_list_t *);
bool analyze_function(ast_t *, ast_symbols*);
bool analyze_statement(ast_t *);
bool analyze_fncall(ast_t *);