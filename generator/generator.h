#include "../ast/ast.h"
#include <stdbool.h>
#include <stdlib.h>

bool generate_code(ast_list_t*);
bool generate_stmts(ast_t*, FILE *);