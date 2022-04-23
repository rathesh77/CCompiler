#ifndef UTILS_H
#define UTILS_H

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

char *copy_name (char *name);
void print_backtrace ();

#endif /* ifndef UTILS_H */
