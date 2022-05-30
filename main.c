#include <stdio.h>

#include "buffer/buffer.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantic/semantic.h"
#include "generator/generator.h"


int main() {
  buffer_t buffer;
  FILE *fd = fopen("sample.txt", "r");
  buf_init(&buffer, fd);


  ast_list_t *ast= parse_code(&buffer);
  if (ast != NULL) {
    if (has_valid_semantic(ast) == true) {
      printf("analyse semantique valide.\n");
      generate_code(ast);
      printf("code généré.\n");

    } else {
      printf("analyse semantique invalide...\n");
    }
  }
  printf("end\n");
  return 0;
}