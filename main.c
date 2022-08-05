#include <stdio.h>
#include <string.h>

#include "buffer/buffer.h"
#include "generator/generator.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantic/semantic.h"

#define EXTENSION "algo"
char * start_compiler(char *);

int main(int argc, char *argv[]) {
  char *code = "aaa";
  code = start_compiler("fonction func (entier a,entier b) : entier { entier ac = 2; }");
  printf("%s", code);
  return 0;
}

char * start_compiler(char *code) {

  buffer_t buffer;
  buf_init(&buffer, BUF_SIZE);
  strcpy(buffer.content, code);
  ast_list_t *ast = parse_code(&buffer);
  
  if (ast != NULL) {
    printf("analyse lexicale et syntaxique valide.\n");

    if (has_valid_semantic(ast) == true) {
      printf("analyse semantique valide.\n");

      char * output = generate_code(ast, output);
      printf("code fini \n");
      return output;
    } else {
      printf("analyse semantique invalide...\n");
    }
  }

  free(ast);

  return NULL;
}