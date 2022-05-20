#include "parser.h"
#include "../buffer/buffer.h"
#include "../lexer/lexer.h"
void parse_code(buffer_t *buffer) {

  // presence d'accolades
  // declarations de fonctions (function func (a : entier, b: entier ...) {})
  // appels de fonctions ( func(a, b) )
  // boucle (for while, do while)
  // condition si else if else ...
  // affectations  (entier a = 5)
  // instructions composés ({ a = 2;b = 3; })
  // ne pas oublier les operateurs unaires (++, --, *=, %=, /=, ...)

  while (!buf_eof(buffer)) {
    char *s = lexer_getalphanum(buffer);
    if ( *s == ' ') {
      
    }
    printf("%c\n", *s);
  }

}