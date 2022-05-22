#include "parser.h"

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
  // pour l'instant on ne prend en compte que le type INTEGER pour les variables
  ast_t *function;
  while (buf_eof(buffer) == false)
  {
    char *lexem = lexer_getalphanum(buffer);
    if (strcmp("fonction", lexem) == 0)
    {
      char *func_name = lexer_getalphanum(buffer);
      char left_parenthesis = buf_getchar_after_blank(buffer);
      ast_list_t *params = malloc(sizeof(ast_list_t));
      ast_list_t *stmts = malloc(sizeof(ast_list_t));
      ast_list_t *ptr_params = params;

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
        //ptr_params = ast_list_add(&ptr_params, ast_new_variable(param_name, 0));
        ptr_params->node = ast_new_variable(param_name, 0);
        ptr_params->next = malloc(sizeof(ast_list_t));
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
  char *lexem = NULL;
  ast_list_t *head = function->function.stmts;
  ast_list_t *cursor =  function->function.stmts;
  while (!buf_eof(buffer)) {
    char end_bracket = buf_getchar_after_blank(buffer);
    //buf_lock(buffer);
    //buf_rollback_and_unlock(buffer, 1);
    if (end_bracket == '}') {
      printf("parsing de fonction terminé\n");
      break;
    }
    buf_lock(buffer);
    buf_rollback_and_unlock(buffer, 1);
    lexem = lexer_getalphanum(buffer);
    ast_t *st = malloc(sizeof(ast_t));
    if (strcmp(lexem, "si") == 0) {
      st = parse_condition(buffer);
    } else if (strcmp(lexem, "pour") == 0) {
      st = parse_loop(buffer);
    } else {
      char next_char = buf_getchar_after_blank(buffer);
      if (next_char == '(') {
        st = parse_fncall(buffer, lexem);
         // function->call.args = malloc(sizeof(ast_list_t));

       //ptr_stmts= ast_list_add(&(ptr_stmts), st);


        cursor->node = st;
        cursor->next = malloc(sizeof(ast_list_t));
        cursor = cursor->next;

      } else if (next_char == '=') {
        st = parse_assignment(buffer);
      } else {
        printf("error");
        return;
      }
    }
  }
  return;
}

ast_t *parse_condition(buffer_t *buffer) {

}

ast_t *parse_loop(buffer_t *buffer) {

}

ast_t * parse_assignment(buffer_t *buffer) {
}

ast_t * parse_fncall(buffer_t *buffer, char *fn_name) {
  ast_t *fn_call = malloc(sizeof(ast_t));
  //fn_call = ast_new_fncall(fn_name)
  ast_list_t *args = malloc(sizeof(ast_list_t));
  ast_list_t *head = args;
  ast_list_t *cursor = args;
  while (true) {
    char *arg = lexer_getalphanum(buffer);
    if (strlen(arg) > 0) {
      char next_char = buf_getchar_after_blank(buffer);
      if (next_char != ',' && next_char != ')') {
        printf("error lors du parsing de la function %s\n", fn_name);
        return NULL;
      }
      //cursor = ast_list_add(&ptr, ast_new_integer(atol(arg)));
      cursor->node = ast_new_integer(atol(arg));
      cursor->next = malloc(sizeof(ast_list_t));
      cursor = cursor->next;      
      if (next_char == ')') {
        break;
      }
    } else {
      break;
    }
   
  }
  fn_call = ast_new_fncall(fn_name, head);
  return fn_call;
}