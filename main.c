#include <stdio.h>
#include <string.h>

#include "buffer/buffer.h"
#include "generator/generator.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "semantic/semantic.h"

#define EXTENSION "algo"

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("Veuillez fournir un fichier format .algo en entrée");
    return -1;
  }
  char *filename = argv[1];
  int len = strlen(filename);
  int cpt = 0;
  char *extension = malloc(sizeof(char) * len);
  while (cpt < len && filename[cpt] != '.') {
    cpt++;
  }
  cpt++;
  if (cpt == len - 1) {
    printf("format de fichier invalide");
    return -1;
  }
  int i = 0;
  while (cpt < len) {
    extension[i] = filename[cpt];
    cpt++;
    i++;
  }
  extension[i] = '\0';

  if (strcmp(extension, EXTENSION) != 0) {
    printf("format de fichier invalide");
    return -1;
  }

  buffer_t buffer;
  FILE *fd = fopen(filename, "r");
  if (fd == NULL) {
    printf("le fichier n\'existe pas");
    return -1;
  }
  buf_init(&buffer, fd);

  ast_list_t *ast = parse_code(&buffer);
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