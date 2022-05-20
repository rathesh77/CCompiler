#include "parser.h"
#include "../buffer/buffer.h"

void parse_code(buffer_t *buffer) {

  while (!buf_eof(buffer)) {
    char s = buf_getchar(buffer);
    if ( s == ' ') {
      
    }
    printf("%c\n", s);
  }

}