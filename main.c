#include <stdio.h>

#include "buffer/buffer.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantic/semantic.h"


int main() {
  buffer_t buffer;
  FILE *fd = fopen("sample.txt", "r");
  buf_init(&buffer, fd);


  ast_list_t *ast= parse_code(&buffer);
  if (ast != NULL) {
    if (iterate_functions(ast) == true) {
      return 0;
    }
  }
  printf("end\n");
  return 0;
}