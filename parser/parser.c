#include "parser.h"

#include <string.h>
#include <stdlib.h>
#include "../utils/utils.h"

ast_t *parse_code(buffer_t *buffer)
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
        return NULL;
      }
      while (true)
      {
        char *param_name = lexer_getalphanum(buffer);
        buf_skipblank(buffer);
        if (buf_getchar(buffer) != ':')
        {
          printf("error");
          return NULL;
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
            return NULL;
          }
          printf("declaration fonction parsé\n");
          function = ast_new_function(func_name, 0, params, stmts);
          parse_function(buffer, function);
          break;
        }
        else if (next_sym != ',')
        {
          printf("error");
          return NULL;
        }
      }
    }
    else
    {
      break;
    }
  }
  return function;
}

void parse_function(buffer_t *buffer, ast_t *function) {
  char *lexem = NULL;
  ast_list_t *head = NULL;
  ast_list_t *cursor = NULL;

  if (function->type == AST_FUNCTION) {
      head = function->function.stmts;
      cursor =  function->function.stmts;

  } else if (function->type == AST_COMPOUND_STATEMENT) {
      head = function->compound_stmt.stmts;
      cursor =  function->compound_stmt.stmts;
  }
  while (!buf_eof(buffer)) {
    char end_bracket = buf_getchar_after_blank(buffer);
    //buf_lock(buffer);
    //buf_rollback_and_unlock(buffer, 1);
    if (end_bracket == '}') {
      printf("parsing de fonction terminé\n");
      return;
    }
    buf_lock(buffer);
    buf_rollback_and_unlock(buffer, 1);
    lexem = lexer_getalphanum(buffer);
    ast_t *st = malloc(sizeof(ast_t));
    if (strcmp(lexem, "si") == 0) {
      st = parse_condition(buffer);
      cursor->node = st;
      cursor->next = malloc(sizeof(ast_list_t));
      cursor = cursor->next;      
    } else if (strcmp(lexem, "tantque") == 0) {
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
  printf("Accolade manquante de fin de scope courante: %s\n",function->function.name);
  return;
}

ast_t *parse_condition(buffer_t *buffer) {
  char left_parenthesis = buf_getchar_after_blank(buffer);
  //buf_unlock(buffer);
  ast_t *wrapper = malloc(sizeof(ast_t));
  ast_t *condition = malloc(sizeof(ast_t));
  ast_t *valid_branch = malloc(sizeof(ast_t));

  valid_branch->type = AST_COMPOUND_STATEMENT;
  valid_branch->compound_stmt.stmts = malloc(sizeof(ast_list_t));

  ast_t *invalid_branch = malloc(sizeof(ast_t));
  
  invalid_branch->type = AST_COMPOUND_STATEMENT;
  invalid_branch->compound_stmt.stmts = malloc(sizeof(ast_list_t));
  
  ast_binary_e operator = {0};

  if (left_parenthesis != '(') {
    printf("error lors du parsing de condition\n");
    return NULL;
  }

  char *lvalue = lexer_getalphanum(buffer);
  char next_char = buf_getchar_after_blank(buffer);
  if (next_char == '<') {
    next_char = buf_getchar(buffer);
    if (next_char == '=') {
      operator.op = "<=";
    } else if (next_char != '\0' && next_char != ' ') {
      printf("error lors du parsing de condition\n");
      return NULL;
    } else {
      operator.op = "<";
    }
  } else if (next_char == '>') {
      next_char = buf_getchar(buffer);
      if (next_char == '=') {
        operator.op = ">=";
      } else if (next_char != '\0' && next_char != ' ') {
        printf("error lors du parsing de condition\n");
        return NULL;
      } else {
          operator.op = ">";
      }
  } else if (next_char == '=' && buf_getchar(buffer) == '=') {
    operator.op = "==";
  }
  char *rvalue = lexer_getalphanum(buffer);
  next_char = buf_getchar_after_blank(buffer);
  if (next_char == ')') {
      condition = ast_new_binary(operator, ast_new_variable(lvalue, 0), ast_new_variable(rvalue, 0));
      if (buf_getchar_after_blank(buffer) != '{') {
          printf("error lors du parsing de condition\n");
          return NULL;
      }
      parse_function(buffer, valid_branch);
      return ast_new_condition(condition, valid_branch, invalid_branch);
      //return ast_new_condition(condition);
  }
  return NULL;
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
    ast_t *arg = parse_arg(buffer, cursor);
    char next_char = buf_getchar_after_blank(buffer);
    if (next_char != ',' && next_char != ')') {
      printf("error lors du parsing de la function %s\n", fn_name);
      return NULL;
    }
    //cursor = ast_list_add(&ptr, ast_new_integer(atol(arg)));
    if (arg != NULL) {
      cursor->node = arg; //ast_new_integer(atol(arg));
      cursor->next = malloc(sizeof(ast_list_t));
      cursor = cursor->next;
    }      
    if (next_char == ')') {
      break;
    }
  }
  fn_call = ast_new_fncall(fn_name, head);
  return fn_call;
}

ast_t *parse_arg(buffer_t *buffer, ast_list_t *cursor) {
    char ch = buf_getchar_after_blank(buffer);
    buf_lock(buffer);
    buf_rollback_and_unlock(buffer, 1);
    if (is_letter(ch) == true) {
      // si premier char est une lettre
      //  alors l'arg est soit un appel de fonction, soit une variable
      // sinon c'est surement un entier hardcodé

      char *arg = lexer_getalphanum(buffer);
      char next_char = buf_getchar_rollback(buffer);
      if (next_char == '(') {
        buf_getchar(buffer);
        return parse_fncall(buffer, arg);
      }
      else if (next_char == ',' || next_char == ')') {
        return ast_new_variable(arg, 0);
      } else {
        printf("error lors du parsing d'un argument: %s\n", arg);
        return NULL;
      }
      

      //char *arg = lexer_getalphanum(buffer);
    } 
    return ast_new_integer(atol(lexer_getalphanum(buffer)));
    
}