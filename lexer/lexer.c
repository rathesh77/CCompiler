#include "../lexer/lexer.h"
#include "../buffer/buffer.h"
#include <string.h>
#include <stdlib.h>
char *concat(const char *s1, const char *s2)
{
  char *result = malloc(strlen(s1) + strlen(s2) + 1);
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}
char *lexer_getalphanum(buffer_t *buffer)
{

  int size = 0;
  char c = buf_getchar(buffer);
  buf_lock(buffer);
  while (
      (c <= '9' && c >= '0') ||
      (c >= 'a' & c <= 'z') ||
      (c >= 'A' && c <= 'Z'))
  {
    buf_print(buffer);
    size++;
    c = buf_getchar(buffer);
  }
  
  if (size == 0)
  {
    printf("end\n");
    buf_rollback(buffer, 1);
    buf_unlock(buffer);
    buf_print(buffer);
    return "";
  }

  char *out = malloc(size);
  char *first_ptr = out;
  printf("size:%d\n", size);
  
  if (!buf_eof(buffer))
    buf_rollback(buffer, size + 1);

  buf_print(buffer);
  while (size > 0)
  {
    c = buf_getchar(buffer);
    // out = strcat(out, &c);
    *out = c;
    out++;
    size--;
  }
  buf_unlock(buffer);
  return first_ptr;
}

char *lexer_getalphanum_rollback(buffer_t *buffer)
{
}

long lexer_getnumber(buffer_t *buffer)
{
}
