#include "parser.h"

#include "../ast/ast.h"
#include "../buffer/buffer.h"
#include "../lexer/lexer.h"
#include <string.h>
#include <stdlib.h>
void parse_code(buffer_t *buffer)
{
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
  while (buf_eof(buffer) == false)
  {
    char *lexem = lexer_getalphanum(buffer);
    if (strcmp("fonction", lexem) == 0)
    {
      char *func_name = lexer_getalphanum(buffer);
      char left_parenthesis = buf_getchar_after_blank(buffer);
      ast_list_t *params = malloc(sizeof(struct ast_list_t));
      ast_list_t *stmts;
      ast_list_t *ptr_params = params;
      ast_list_t *ptr_stmts = stmts;

      if (left_parenthesis != '(')
      {
        printf("error");
        return;
      }
      while (true)
      {
        char *param_name = lexer_getalphanum(buffer);
        buf_skipblank(buffer);
        if (buf_getchar(buffer) != ':')
        {
          printf("error");
          return;
        }
        char *param_type = lexer_getalphanum(buffer);
        ptr_params->node = ast_new_variable(param_name, 0);
        ptr_params->next = malloc(sizeof(struct ast_list_t));
        ptr_params = ptr_params->next;
        char next_sym = buf_getchar_after_blank(buffer);
        if (next_sym == ')')
        {
          if (buf_getchar_after_blank(buffer) != '{') {
            printf("error");
            return;
          }
          printf("declaration fonction parsé\n");
          function = ast_new_function(func_name, 0, params, stmts);
          parse_function(buffer, function);
          break;
        }
        else if (next_sym != ',')
        {
          printf("error");
          return;
        }
      }
    }
    else
    {
      break;
    }
  }
  printf("end\n");
}

void parse_function(buffer_t *buffer, ast_t *function) {

}