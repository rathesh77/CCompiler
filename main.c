#include <stdio.h>
#include "buffer/buffer.h"
#include "lexer/lexer.h"
#define MAX_BUFFER_SIZE = 1024

int main() {


  buffer_t buffer; 
  FILE *fd = fopen("sample.txt", "r");
  buf_init(&buffer, fd); 
  char *out = lexer_getalphanum(&buffer);
  printf("%s", out);

  return 0;
}