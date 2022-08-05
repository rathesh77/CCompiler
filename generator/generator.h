#include "../ast/ast.h"
#include <stdbool.h>
#include <stdlib.h>


char *output;
char * generate_code(ast_list_t*, char*);
void generate_stmts(ast_list_t*, int);
char *build_expr(ast_t*);
char *build_fncall(ast_t*);
char* convert_operator(char *op);
void indent(int);
