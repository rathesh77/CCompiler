#include <stdlib.h>
#include <string.h>
#include "utils.h"
#ifndef WIN32
#include <unistd.h>
#include <execinfo.h>
#endif

char *copy_name (char *name)
{
  size_t len = strlen(name) + 1;
  char *out = malloc(sizeof(char) * len);
  strncpy(out, name, len);
  return out;
}

void print_backtrace ()
{
#ifndef WIN32
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif /* WIN32 */
}

bool is_letter(char c) {
  return ((c) >= ('a') && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_arithmetic_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

bool is_logic_operator(char* c) {
      return strcmp(c, "ET") == 0 || strcmp(c, "OU") == 0;
}