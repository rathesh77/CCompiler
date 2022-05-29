#include "../ast/ast.h"
#include "../buffer/buffer.h"
#include "../lexer/lexer.h"

void parse_function(buffer_t *, ast_t **);

ast_list_t *parse_code(buffer_t *);
ast_t *parse_condition(buffer_t *, ast_t *);
ast_t *parse_loop(buffer_t *);
ast_t *parse_assignment(buffer_t *, char*);
ast_t *parse_fncall(buffer_t *, char *);
ast_t *parse_arg(buffer_t *);
ast_t *parse_expr(buffer_t *);
ast_t *NPI(ast_list_t*, int);
ast_t *ast_from_stack(ast_t *[], ast_t*, int*);
bool is_higher_precedence(ast_t *, ast_t *);