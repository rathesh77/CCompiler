#include "parser.h"

#include "../ast/ast.h"
#include "../buffer/buffer.h"
#include "../lexer/lexer.h"
#include <string.h>

void parse_code(buffer_t *buffer) {
  // presence d'accolades
  // declarations de fonctions (fonction func (a : entier, b: entier ...) {})
  // appels de fonctions ( func(a, b) )
  // boucle (pour, tant que, faire tant que)
  // condition si sinon, si sinon si sinon ...
  // affectations  (entier a = 5)
  // instructions composés ({ a = 2;b = 3; })
  // ne pas oublier les operateurs unaires (++, --, *=, %=, /=, ...)

  // on cherche d'abord une declaration de fonction, si y en a pas, on throw une
  // erreur
  ast_t *function;
  while (buf_eof(buffer) == false) {
    char *lexem = lexer_getalphanum(buffer);
    if (strcmp("fonction", lexem) == 0) {
      char *func_name = lexer_getalphanum(buffer);
      buf_skipblank(buffer);
      char left_parenthesis = buf_getchar(buffer);
      ast_list_t *params;
      ast_list_t *stmts;
      ast_list_t *ptr_param = params;
      ast_list_t *ptr_stmts = stmts;

      if (left_parenthesis != '(') {
        printf("error");
        return;
      }
      while (true) {
        lexem = lexer_getalphanum(buffer);
        char *param_name = lexer_getalphanum(buffer);
        if (lexer_getalphanum(buffer)[0] != ':') {
          printf("error");
          return;
        }
        char *param_type = lexer_getalphanum(buffer);
        ptr_param->node = ast_new_variable(param_name, param_type == "entier" ? 1 : 0);
        ptr_param = ptr_param->next;
        char *next_sym = lexer_getalphanum(buffer);
        if (*next_sym == ')') {
          printf("declaration fonction parsé");
          function = ast_new_function(func_name, 0, params, stmts);
          break;

        } else if (*next_sym != ',') {
          printf("error");
          return;
        }
      }
    } else {
      break;
    }
  }
  printf("end\n");
}
