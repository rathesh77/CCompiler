#include "../ast/ast.h"
#include <stdbool.h>
#include <stdlib.h>

bool generate_code(ast_list_t*);
bool generate_stmts(ast_list_t*, FILE *, int);
char *build_expr(ast_t*);
char *build_fncall(ast_t*);
char* convert_operator(char *op);
void indent(FILE *,int);