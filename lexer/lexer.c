#include "../lexer/lexer.h"

#include <stdlib.h>
#include <string.h>

#include "../buffer/buffer.h"

bool is_num(char c) { return c >= '0' && c <= '9'; }

bool is_letter(char c) {
  return (c >= 'a' & c <= 'z') || (c >= 'A' && c <= 'Z');
}

char *concat(const char *s1, const char *s2) {
  char *result = malloc(strlen(s1) + strlen(s2) + 1);
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

char *lexer_getalphanum(buffer_t *buffer) {
  int size = 0;
  
  buf_skipblank(buffer);
  buf_lock(buffer);
  char c = buf_getchar(buffer);

  int end = 0;
  int offset = 1;
  while (is_num(c) || is_letter(c)) {
    size++;

    if (buffer->avail == 0) {
      offset = 0;
      break;
    }

    c = buf_getchar(buffer);
  }

  if (size == 0) {
    printf("fin de ligne\n");
    buf_getchar_rollback(buffer);
    buf_unlock(buffer);
    //buf_print(buffer);
    return "\0";
  }

  char *out = malloc(size);
  char *first_ptr = out;
  //printf("size:%d\n", size);

  buf_rollback(buffer, size + offset);

  while (size > 0) {
    c = buf_getchar(buffer);
    *out = c;
    out++;
    size--;
  }
  *out = '\0';
  buf_unlock(buffer);
  return first_ptr;
}

char *lexer_getalphanum_rollback(buffer_t *buffer) {
  int initial_pos = buffer->it;
  char *out = lexer_getalphanum(buffer);
  buf_lock(buffer);
  buf_rollback(buffer, buffer->it - initial_pos);
  buf_unlock(buffer);
  // printf("%d, %d\n", buffer->it, initial_pos);
  return out;
}

long lexer_getnumber(buffer_t *buffer) {
  buf_skipblank(buffer);
  buf_lock(buffer);

  char ch = buf_getchar(buffer);
  int count = 0;
  if (is_num(ch) || ch == '-') {
    count++;
  } else {
    return 0;
  }

  while (true) {
    count++;
    if (buffer->avail == 0) break;
    ch = buf_getchar(buffer);
    if (!is_num(ch)) {
      break;
    }
  }
  buf_rollback(buffer, count);
  char *out = malloc(count);
  char *temp = out;

  while (count > 0) {
    ch = buf_getchar(buffer);
    *out = ch;
    out++;
    count--;
  }
  *out = '\0';
  buf_unlock(buffer);
  return strtol(temp, NULL, 10);
}