#include <stdlib.h>
#include <string.h>
#include "utils.h"
#ifndef WIN32
#include <unistd.h>
#endif
#include "../dictionnary/dictionnary.h"


bool is_letter(char c) {
  return ((c) >= ('a') && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool is_arithmetic_operator(char c) {
    return c == PLUS || c == MINUS || c == MULT || c == DIVISION;
}

bool is_logic_operator(char* c) {
      return strcmp(c, AND) == 0 || strcmp(c, OR) == 0;
}
