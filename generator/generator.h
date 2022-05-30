#include "../ast/ast.h"
#include <stdbool.h>
#include <stdlib.h>

bool generate_code(ast_list_t*);
bool generate_stmts(ast_list_t*, FILE *, int);
char *build_expr(ast_t*);
char *concat_expr(char *, char *);

char* convert_operator(char *op);
