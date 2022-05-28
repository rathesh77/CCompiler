#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../utils/utils.h"
#include "../dictionnary/dictionnary.h"
#include "parser.h"
#include "../precedence_table.h"

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
  // NE PAS OUBLIER DE GERER LES TYPES DE RETOUR DES FONCTIONS
  ast_t *function;
  while (buf_eof(buffer) == false)
  {
    char *lexem = lexer_getalphanum(buffer);
    if (strcmp(FUNCTION, lexem) == 0)
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
        ptr_params->node = ast_new_variable(param_name, 0);
        ptr_params->next = malloc(sizeof(ast_list_t));
        ptr_params = ptr_params->next;
        char next_sym = buf_getchar_after_blank(buffer);
        if (next_sym == ')')
        {
          if (buf_getchar_after_blank(buffer) != ':') {
            printf("Type de retour requis\n");
            return NULL;
          }
          char *ret = lexer_getalphanum(buffer);
          if (strcmp(ret, INTEGER) != 0 && strcmp(ret, VOID) != 0) {
            printf("Type de retour inconnu\n");
            return NULL;
          }
          if (buf_getchar_after_blank(buffer) != '{') {
            printf("Accolade ouvrante requis\n");
            return NULL;
          }
          printf("declaration fonction parsé\n");
          function = ast_new_function(func_name, strcmp(ret, INTEGER) == 0 ? 0 : -1, params, stmts);
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
  ast_list_t *cursor = NULL;
  ast_t *previous = NULL;

  if (function->type == AST_FUNCTION) {
      cursor =  function->function.stmts;

  } else if (function->type == AST_COMPOUND_STATEMENT) {
      cursor =  function->compound_stmt.stmts;
  } else if (function->type == AST_LOOP) {
      cursor =  function->loop.stmts;
  } 
  while (!buf_eof(buffer)) {
    char end_bracket = buf_getchar_after_blank(buffer);
    if (end_bracket == '}') {
      printf("parsing de fonction terminé\n");
      return;
    }
    buf_lock(buffer);
    buf_rollback_and_unlock(buffer, 1);
    lexem = lexer_getalphanum(buffer);
    ast_t *st = malloc(sizeof(ast_t));
    if (strcmp(lexem, IF) == 0) {
      char left_parenthesis = buf_getchar_after_blank(buffer);
      if (left_parenthesis != '(') {
        printf("error lors du parsing de condition\n");
        return;
      }      
      st = parse_expr(buffer);
      if (st == NULL) {
        return;
      }
      if (buf_getchar_after_blank(buffer) == ')') {
        if (buf_getchar_after_blank(buffer) != '{') {
          printf("Accolade fermante manquante apres la condition si\n");
          return;
        }
      } else {
           printf("Accolade fermante manquante apres la condition si\n");
          return;
      }

      ast_t *valid_branch = malloc(sizeof(ast_t));

      valid_branch->type = AST_COMPOUND_STATEMENT;
      valid_branch->compound_stmt.stmts = malloc(sizeof(ast_list_t));

      ast_t *invalid_branch = malloc(sizeof(ast_t));
      
      invalid_branch->type = AST_COMPOUND_STATEMENT;
      invalid_branch->compound_stmt.stmts = malloc(sizeof(ast_list_t));
  
      
      parse_function(buffer, valid_branch);
      st = ast_new_condition(st, valid_branch, invalid_branch);

      cursor->node = st;
      previous = st;
      cursor->next = malloc(sizeof(ast_list_t));
      cursor = cursor->next;

    } else if (strcmp(lexem, "tantque") == 0) {

      if (buf_getchar_after_blank(buffer) != '(') {
        printf("parenthese ouvrante manquante au debut d\'une tantque\n");
        return;
      }
      st = parse_expr(buffer);
      ast_t *loop = ast_new_loop(st, malloc(sizeof(ast_list_t)));
        if (buf_getchar_after_blank(buffer) == ')') {
        if (buf_getchar_after_blank(buffer) != '{') {
          printf("Accolade fermante manquante apres la condition si\n");
          return;
        }
      } else {
           printf("Accolade fermante manquante apres la condition si\n");
          return;
      }
      parse_function(buffer, loop);
      cursor->node = loop;
      previous = loop;
      cursor->next = malloc(sizeof(ast_list_t));
      cursor = cursor->next;
    } else if(strcmp(lexem, ELSE_IF) == 0) {
      char left_parenthesis = buf_getchar_after_blank(buffer);
      if (left_parenthesis != '(') {
        printf("error lors du parsing de condition\n");
        return;
      }            
      if (previous->type != AST_CONDITION) {
        printf("peut pas utiliser une sinonsi sans une precedente sinon\n");
        return;
      }

      st = parse_expr(buffer);
      if (buf_getchar_after_blank(buffer) == ')') {
        if (buf_getchar_after_blank(buffer) != '{') {
          printf("Accolade fermante manquante apres la condition sinonsi\n");
          return;
        }
      } else {
          printf("Accolade fermante manquante apres la condition sinonsi\n");
          return;
      }
      ast_t *valid_branch = malloc(sizeof(ast_t));

      valid_branch->type = AST_COMPOUND_STATEMENT;
      valid_branch->compound_stmt.stmts = malloc(sizeof(ast_list_t));

      ast_t *invalid_branch = malloc(sizeof(ast_t));
      
      invalid_branch->type = AST_COMPOUND_STATEMENT;
      invalid_branch->compound_stmt.stmts = malloc(sizeof(ast_list_t));
      parse_function(buffer, valid_branch);
        
      st = ast_new_condition(st, valid_branch, invalid_branch);
      previous->branch.invalid = st;
      previous = st;
    } else if (strcmp(lexem, ELSE) == 0) {
      if (previous->type != AST_CONDITION) {
        printf("peut pas utiliser une sinon sans une si/sinonsi\n");
        return;
      }
      if (buf_getchar_after_blank(buffer) != '{') {
        printf("Accolade fermante manquante apres la condition si\n");
        return;
      }
      parse_function(buffer, previous->branch.invalid);
    } else {
      int count = buffer->it;
      int temp = count;
      int found = 0;
      while (true) {
        char  c = buf_getchar(buffer);
        if (c == '\n')
          break;
        if (c== ';') {
          count = buffer->it;
          found = 1;
          break;
        }
        count = buffer->it;
      }
      if (found == 0) {
          printf("point virgule manquant\n");
          free(function);
          return;
      }
      buf_lock(buffer);
      buf_rollback_and_unlock(buffer, count - temp);
      char next_char = buf_getchar_after_blank(buffer);
      if (next_char == '(') {
        st = parse_fncall(buffer, lexem);
        if (buf_getchar_after_blank(buffer) != ';') {
          printf("point virgule manquant\n");
          return;
        }

      } else if (next_char == '=') {
        if (!is_letter(lexem[0])) {
          printf("LHS must be a variable of this scope\n");
          return;
        }
        st = parse_assignment(buffer, lexem);
      } else {
        printf("error");
        return;
      }
        cursor->node = st;
        previous = st;
        cursor->next = malloc(sizeof(ast_list_t));
        cursor = cursor->next;
    }
    if (st == NULL) {
      free(function);
      return;
    }
  }
  free(function);
  printf("Accolade manquante de fin de scope courante: %s\n",function->function.name);
  return;
}

ast_t * parse_assignment(buffer_t *buffer, char * var_name) {

  ast_t *right = parse_expr(buffer);
  
  char semi_colon = buf_getchar_after_blank(buffer);
  if (semi_colon == ';') 
    return ast_new_assignment(ast_new_variable(var_name, 0), right);
  
  printf("error lors du parsing d\'assignement\n");
  return NULL;
  
}

ast_t * parse_fncall(buffer_t *buffer, char *fn_name) {
  ast_t *fn_call = malloc(sizeof(ast_t));
  ast_list_t *args = malloc(sizeof(ast_list_t));
  ast_list_t *head = args;
  ast_list_t *cursor = args;
  while (true) {
    ast_t *arg = parse_expr(buffer);
    char next_char = buf_getchar_after_blank(buffer);
    if (next_char != ',' && next_char != ')') {
      printf("error lors du parsing de la function %s\n", fn_name);
      return NULL;
    }
    if (arg != NULL) {
      cursor->node = arg;
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

ast_t *parse_arg(buffer_t *buffer) {
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
        return ast_new_variable(arg, 0);
    } 
    return ast_new_integer(atol(lexer_getalphanum(buffer)));
    
}

ast_t *parse_expr(buffer_t *buffer) {

  ast_list_t *head= malloc(sizeof(ast_list_t));
  ast_list_t *cursor = head;
  ast_list_t *previous = head;

  int len = 0;
  while (true) {
    char next_char = buf_getchar_after_blank(buffer);
    if (next_char == ';' || next_char == ',') {
      buf_lock(buffer);
      buf_rollback_and_unlock(buffer, 1);
      break;
    } else if (next_char == '(') {
      ast_t *arg = parse_expr(buffer);
      if ( arg == NULL || buf_getchar_after_blank(buffer) != ')') {
        printf("manque une parenthese fermante!\n");
        return NULL;
      }
      ast_t* unary_parenthese = malloc(sizeof(ast_t));
      unary_parenthese->type = AST_UNARY;
      unary_parenthese->unary.op = "(";
      unary_parenthese->unary.operand = arg;
      cursor->node = unary_parenthese;
      cursor->next = malloc(sizeof(ast_list_t));
      previous = cursor;
      cursor = cursor->next;
      len++;
      continue;
    } else if (next_char == ')') {
      buf_lock(buffer);
      buf_rollback_and_unlock(buffer, 1);
      break;
    }
    else if (is_arithmetic_operator(next_char)) {  // si on tombe sur +, - ,/ ou *
      ast_binary_e operator = {};
      operator.op = malloc(sizeof(char) * 2);
      operator.op[0] = next_char;
      operator.op[1] = '\0';
      ast_t *bin = ast_new_binary(operator, NULL, NULL);
      cursor->node = bin;
      cursor->next = malloc(sizeof(ast_list_t));
      previous = cursor;
      cursor = cursor->next;
      len++;
      continue;
    }
    buf_lock(buffer);
    buf_rollback_and_unlock(buffer, 1);    
    char *lexem = lexer_getalphanum(buffer);
    if (is_logic_operator(lexem)) {         // si on tombe sur "ET" ou "OU"
        ast_binary_e operator = {};
        operator.op = lexem;

        ast_t *bin = ast_new_binary(operator, NULL, NULL);
        cursor->node = bin;
        cursor->next = malloc(sizeof(ast_list_t));
        previous = cursor;
        cursor = cursor->next;
        len++;
        continue;      
    }
    if (strlen(lexem) == 0) {
        char next_char = buf_getchar(buffer);
        char *comp_op = malloc(sizeof(char) * 2);
        if (next_char == '<') {
          next_char = buf_getchar(buffer);
          if (next_char == '=') {
            comp_op = "<=";
          } else if (next_char != '\0' && next_char != ' ') {
            printf("error lors du parsing de condition\n");
            return NULL;
          } else {
            comp_op = "<";
          }
        } else if (next_char == '>') {
            next_char = buf_getchar(buffer);
            if (next_char == '=') {
              comp_op = ">=";
            } else if (next_char != '\0' && next_char != ' ') {
              printf("error lors du parsing de condition\n");
              return NULL;
            } else {
                comp_op= ">";
            }
        } else if (next_char == '=' && buf_getchar(buffer) == '=') {
          comp_op = "==";
        } else {
          printf("error\n");
          return NULL;
        }
        ast_binary_e operator = {};
        operator.op = comp_op;

        ast_t *bin = ast_new_binary(operator, NULL, NULL);
        cursor->node = bin;
        cursor->next = malloc(sizeof(ast_list_t));
      previous = cursor;

        cursor = cursor->next;
        len++;
        continue;
    }
    buf_lock(buffer);
    buf_rollback_and_unlock(buffer, strlen(lexem));
    ast_t *arg = parse_arg(buffer);
    cursor->node = arg;
    cursor->next = malloc(sizeof(ast_list_t));
      previous = cursor;

    cursor = cursor->next;
    len++;

  }
  if (len % 2 == 0) {
    printf("nombre insuffisant de membre dans l\'expression\n");
    return NULL;
  }
  previous->next = NULL;
  return NPI(head, len);
}

ast_t *NPI(ast_list_t* expr, int len) {
  if (len ==1 ) {
    return expr->node;
  }
  if (expr == NULL) {
    return NULL;
  }

  ast_list_t *cursor = expr;
  ast_t *stack[len+1];
  ast_t *out[len];

  int i = 0;
  int cpt = len -1;
  stack[0] = NULL;
  while (cursor != NULL && i < len) {

    //if (cursor->node->type == AST_BINARY) {
      if (stack[i] == NULL || is_higher_precedence(cursor->node, stack[i])) {
        stack[++i] = cursor->node;
        cursor = cursor->next;
      } else {
   
        do {
          out[cpt--] = stack[i];
          stack[i] = NULL;
          i--;
        } while (stack[i] != NULL && is_higher_precedence(stack[i], out[cpt+1]));
        //stack[i] = cursor->node;
      }
    //} 
  }
  while (i > 0) {
    out[cpt--] = stack[i--];
  }
  cpt++;
  /*ast_t *final = malloc(sizeof(ast_t));
  final->type = AST_BINARY;

  ast_t *fcursor = final;
  fcursor->binary.op = out[cpt++]->binary.op;

  while (cpt < len) {
    ast_t *right = NULL;
    ast_t *left = NULL;    
    
    right = out[cpt++];
    fcursor->binary.right = right;

    if (cpt == len) {
      return final;
    }

    left = out[cpt++];
    fcursor->binary.left = left;
    fcursor = fcursor->binary.left;
  }
  return final;*/
  int x = 0;
  ast_binary_e e;
  return ast_from_stack(out, ast_new_binary(e, NULL, NULL), &x);
}

ast_t *ast_from_stack(ast_t *stack[], ast_t *tree, int *i) {
  if (stack[*i]->type == AST_VARIABLE || 
  stack[*i]->type == AST_INTEGER || 
  (stack[*i]->type == AST_UNARY && strcmp(stack[*i]->unary.op, "(") == 0)) {
    return stack[*i];
  }

  if (stack[*i]->type == AST_BINARY) {
    if (*i == 0)
      tree->type = AST_BINARY;
    tree = stack[*i];
    if (tree->binary.right == NULL) {
      (*i)++;
      tree->binary.right = ast_from_stack(stack, tree->binary.right, i);
    }

    if (tree->binary.left == NULL) {
      (*i)++;

      tree->binary.left = ast_from_stack(stack, tree->binary.left, i);
    }
  }
  return tree;
}

bool is_higher_precedence(ast_t *a, ast_t *b) {
  char *left = malloc(sizeof(char)* 3);
  char *right =  malloc(sizeof(char)* 3);  
  if (a->type == AST_BINARY) {
    left = a->binary.op.op;  
  } else if (a->type == AST_INTEGER || a->type == AST_VARIABLE) {
        left= "VAR";

  } else if (a->type == AST_UNARY && strcmp(a->unary.op, "(") == 0) {
        left= "(";
  }

  if (b->type == AST_BINARY) {
    right = b->binary.op.op;  
  } else if (b->type == AST_INTEGER || b->type == AST_VARIABLE){
    right = "VAR";
  } 
  else if (b->type == AST_UNARY && strcmp(b->unary.op, "(") == 0) {
        right = "(";
  }
  int left_rank = TABLE_LEVELS;
  int right_rank = TABLE_LEVELS;

  for (int i = 0; i < TABLE_LEVELS; i++) {
    for (int j = 0; j < 4; j++) {
      if (table[i][j] == NULL) {
        break;
      }
      if (strcmp(table[i][j], left) == 0) {
        left_rank = TABLE_LEVELS - i;
      }
      if (strcmp(table[i][j],right) == 0) {
        right_rank = TABLE_LEVELS - i;
      }
    }
  }

  return left_rank > right_rank;
}