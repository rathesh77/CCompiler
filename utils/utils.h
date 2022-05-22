#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#define DEBUG true
// #define DEBUG false

#define COLOR_BLUE "\e[34m"
#define COLOR_GREEN "\e[32m"
#define COLOR_RED "\e[31m"
#define COLOR_BG_RED "\e[41m"
#define COLOR_BG_BLUE "\e[44m"
#define COLOR_BG_GREEN "\e[42m"
#define COLOR_DEFAULT "\e[39m\e[49m"

#define STREQUAL 0

bool is_letter(char c);
char *copy_name (char *);
void print_backtrace ();
bool is_arithmetic_operator(char);
bool is_logic_operator(char*);

#endif /* ifndef UTILS_H */
