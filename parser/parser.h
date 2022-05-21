#include "../buffer/buffer.h"
#include "../ast/ast.h"
#include "../buffer/buffer.h"
#include "../lexer/lexer.h"

void parse_code(buffer_t *);
void parse_function(buffer_t *, ast_t *);
ast_t * parse_condition(buffer_t *);
ast_t * parse_loop(buffer_t *);
ast_t * parse_assignment(buffer_t *);
ast_t * parse_fncall(buffer_t *, char *);