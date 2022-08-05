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
  code = start_compiler("fonction func (entier a,entier b) : entier {"
    "entier ac = 2;"
    "entier Y;"
    "entier UI;"
    "entier X;"
    "entier Z;"
    "entier d;"
    "entier F;"
    "entier P;"
    "entier C;"
    "entier test = (a <= b) ET (( Y >= UI OU ((X  == Z)))) OU d == F ET (P == C);"
    "si ( test == 2 ET func(func(15,3), func(23,2)) == 2) {"
    "    entier test1 = func(func(func(func(15,3), func(2,func(func((a <= b) ET (( Y >= UI OU ((X  == Z)))) OU d == F ET (P == C),3), func(2,4)))),3), func(2,func(func(122,3), func(2,2))));"
    "} sinonsi (test >= test) {"
    "    entier test2 = 4;"
    "} "
    "sinonsi (test >= test) {"
    "    entier ac1 = 2;"
    "    si ( test == 2 ET func(func(15,3), func(23,2)) == 2) {"
    "        entier test3 = 1;"
    "    tantque (test > func(5 + 89, 3)) {"
    "        entier test4s = func(func(15,3), func(23,2));"
    "        entier dodso = 4;"
    "        si ( (((20 + 4)) * 134 + 489 +(5 / 94) * 5) +178 ) {"
    "            entier tesst5 = 5;"
    "            entier tests4s = func(func(test4s,3), func(23,2));"
    "        } "
    "    "
    "    }"
    "    }"
    "} "
    "sinon {"
    "    entier ac2 = 2;"
    "} "
  "}");
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

      char *out = generate_code(ast);
      printf("code fini \n");
      return out;
    } else {
      printf("analyse semantique invalide...\n");
    }
  }

  free(ast);

  return NULL;
}