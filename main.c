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
  char *file_without_extension = malloc(sizeof(char) * len);

  int cpt = 0;
  char *extension = malloc(sizeof(char) * len);
  while (cpt < len && filename[cpt] != '.') {
    file_without_extension[cpt] = filename[cpt];
    cpt++;
  }
  file_without_extension[cpt] = '\0';
  if (cpt == len) {
    printf("format de fichier invalide");
    return -1;
  }
  cpt++;
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
  fclose(fd);

  if (ast != NULL) {
    if (has_valid_semantic(ast) == true) {
      printf("analyse semantique valide.\n");
      FILE * output;
      char *output_filename = malloc(sizeof(char) * (strlen(file_without_extension) + strlen(EXTENSION) + 1));      
      strcpy(output_filename, file_without_extension);
      strcat(output_filename, ".js");
      output = fopen(output_filename, "w");
      if(output == NULL) {
          printf("Impossible de créer le fichier\n");
      } else {
        generate_code(ast, output);
        fclose(output);
        printf("code généré dans le fichier ./%s\n", output_filename);
      }
      free(output_filename);

    } else {
      printf("analyse semantique invalide...\n");
    }
  }

  free(ast);

  printf("end\n");
  return 0;
}