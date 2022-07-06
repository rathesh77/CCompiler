#include "parser.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../dictionnary/dictionnary.h"
#include "../operators_precedence/operators_table.h"
#include "../utils/utils.h"

ast_t *parse_return(buffer_t *buffer) {
  char *lexem = lexer_getalphanum(buffer);
  if (strcmp(lexem, RETURN) != 0) {
    return NULL;
  }
  ast_t *expr = parse_expr(buffer);
  if (expr == NULL) {
    return NULL;
  }
  if (buf_getchar_after_blank(buffer) != ';') {
    return NULL;
  }
  return ast_new_return(expr);
}
ast_list_t *parse_code(buffer_t *buffer) {
  
  ast_list_t *functions = malloc(sizeof(ast_list_t));
  ast_list_t *cursor = functions;

  while (true) {
    buf_skipblank(buffer);
    if (buffer->it == buffer->size) 
      break;
    char *lexem = lexer_getalphanum(buffer);
    if (strcmp(FUNCTION, lexem) == 0) {
      char *func_name = lexer_getalphanum(buffer);
      char left_parenthesis = buf_getchar_after_blank(buffer);
      ast_list_t *params = malloc(sizeof(ast_list_t));
      ast_list_t *stmts = malloc(sizeof(ast_list_t));
      ast_list_t *ptr_params = params;
      bool has_more_params = false;
      if (left_parenthesis != '(') {
        printf("error");
        return NULL;
      }
      while (true) {
        char *param_type = lexer_getalphanum(buffer);
        if (strlen(param_type) == 0) {
          if (has_more_params == true) {
            printf("error\n");
            return NULL;
          }
          char next_char = buf_getchar_after_blank(buffer);
          if (next_char == ')') {
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
            ptr_params->node = ast_new_return(NULL);

            ptr_params->node->type = AST_NULL;
            ast_t *function = ast_new_function(
                func_name, strcmp(ret, INTEGER) == 0 ? 0 : -1, params, stmts);
            parse_function(buffer, &function);
            if (function == NULL) {
              free(functions);
              functions = NULL;
              return NULL;
            }

            cursor->node = function;
            cursor->next = malloc(sizeof(ast_t));
            cursor = cursor->next;
            break;
          } else {
            printf("error\n");
            return NULL;
          }
        }
        if (strcmp(param_type, "entier") != 0) {
          printf("parametre de fonction manquant\n");
          return NULL;
        }
        char *param_name = lexer_getalphanum(buffer);
        if (strlen(param_name) == 0) {
          printf("parametre de fonction manquant\n");
          return NULL;
        }
        ptr_params->node = ast_new_variable(param_name, 0);
        ptr_params->next = malloc(sizeof(ast_list_t));
        ptr_params = ptr_params->next;
        if (buf_getchar_after_blank(buffer) == ',') {
          has_more_params = true;
        } else {
          has_more_params = false;

          buf_lock(buffer);
          buf_rollback_and_unlock(buffer, 1);
        }
      }
    } else {
      printf("signature de fonction incorrecte\n");
      return NULL;
    }
  }
  cursor->node = ast_new_return(NULL);
  cursor->node->type = AST_NULL;

  return functions;
}

void parse_function(buffer_t *buffer, ast_t **function) {
  char *lexem = NULL;
  ast_list_t *cursor = NULL;
  ast_t *previous = NULL;

  if ((*function)->type == AST_FUNCTION) {
    cursor = (*function)->function.stmts;

  } else if ((*function)->type == AST_COMPOUND_STATEMENT) {
    cursor = (*function)->compound_stmt.stmts;
  } else if ((*function)->type == AST_LOOP) {
    cursor = (*function)->loop.stmts;
  }
  while (!buf_eof(buffer)) {
    char end_bracket = buf_getchar_after_blank(buffer);
    if (end_bracket == '}') {
      cursor->node = ast_new_return(NULL);
      cursor->node->type = AST_NULL;
      return;
    }
    buf_lock(buffer);
    buf_rollback_and_unlock(buffer, 1);
    ast_t *st = NULL;

    int prev = buffer->it;
    st = parse_return(buffer);
    if (st == NULL) {
      buf_lock(buffer);
      buf_rollback_and_unlock(buffer, buffer->it - prev);
    } else {
      cursor->node = st;
      previous = st;
      cursor->next = malloc(sizeof(ast_list_t));
      cursor = cursor->next;
      continue;
    }
    st = parse_condition(buffer);
    if (st == NULL) {
      buf_lock(buffer);
      buf_rollback_and_unlock(buffer, (buffer->it - prev));
    } else {
      cursor->node = st;
      previous = st;
      cursor->next = malloc(sizeof(ast_list_t));
      cursor = cursor->next;
      ast_t *elseif = NULL;
      while (true) {
        char *lexem = lexer_getalphanum(buffer);
        if (strcmp(lexem, ELSE_IF) != 0) {
          buf_lock(buffer);
          buf_rollback_and_unlock(buffer, strlen(lexem));
          break;
        }
        elseif = is_elseif(buffer, previous);
        if (elseif == NULL) {
          break;
        }
        previous = elseif;
      }
        char *lexem = lexer_getalphanum(buffer);
        if (strcmp(lexem, ELSE) != 0) {
          buf_lock(buffer);
          buf_rollback_and_unlock(buffer, strlen(lexem));
        } else {
         if (buf_getchar_after_blank(buffer)!= '{') {
           break;
         }
         ast_t *c = malloc(sizeof(ast_t));
         c->type = AST_COMPOUND_STATEMENT;
         c->compound_stmt.stmts = malloc(sizeof(ast_list_t));
         previous->branch.invalid = ast_new_comp_stmt(c->compound_stmt.stmts);
         parse_function(buffer,&(c));
        }
      
      continue;
    }
    st = parse_loop(buffer);
    if (st == NULL) {
      buf_lock(buffer);
      buf_rollback_and_unlock(buffer, buffer->it - prev);
    } else {
      cursor->node = st;
      previous = st;
      cursor->next = malloc(sizeof(ast_list_t));
      cursor = cursor->next;
      continue;
    }
    st = parse_assignment(buffer);
    if (st == NULL) {
      buf_lock(buffer);
      buf_rollback_and_unlock(buffer, buffer->it - prev);
    } else {
      cursor->node = st;
      previous = st;
      cursor->next = malloc(sizeof(ast_list_t));
      cursor = cursor->next;
      continue;
    }
    st = parse_declaration(buffer);
    if (st == NULL) {
      buf_lock(buffer);
      buf_rollback_and_unlock(buffer, buffer->it - prev);
    } else {
      cursor->node = st;
      previous = st;
      cursor->next = malloc(sizeof(ast_list_t));
      cursor = cursor->next;
      continue;
    }
    st = is_fncall(buffer);

    if (st == NULL) {
      buf_lock(buffer);
      buf_rollback_and_unlock(buffer, buffer->it - prev);
    } else {
      cursor->node = st;
      previous = st;
      cursor->next = malloc(sizeof(ast_list_t));
      cursor = cursor->next;
    }
    if (st == NULL) {
      break;
    }
  }
  printf("Une ou plusieurs erreurs de syntaxe rencontrées\n");
  free(*function);
  *function = NULL;
  function = NULL;

  return;
}

ast_t *parse_assignment(buffer_t *buffer) {
  char *var_name = lexer_getalphanum(buffer);
  if (buf_getchar_after_blank(buffer) != '=') {
    return NULL;
  }
  ast_t *right = parse_expr(buffer);
  if (right == NULL || right->type == AST_NULL) {
    printf("error lors du parsing de declaration\n");
    return NULL;
  }
  char semi_colon = buf_getchar_after_blank(buffer);
  if (semi_colon == ';')
    return ast_new_assignment(ast_new_variable(var_name, 0), right);

  printf("error lors du parsing d\'assignement\n");
  return NULL;
}

ast_t *parse_declaration(buffer_t *buffer) {
  if (strcmp(lexer_getalphanum(buffer), INTEGER) != 0) {
    return NULL;
  }
  char *var_name = lexer_getalphanum(buffer);
  if (strlen(var_name) == 0) {
    return NULL;
  }
  if (buf_getchar_after_blank(buffer) != '=') {
    return ast_new_declaration(ast_new_variable(var_name, 0), NULL);
  }
  ast_t *right = parse_expr(buffer);
  if (right == NULL || right->type == AST_NULL) {
    printf("error lors du parsing de declaration\n");
    return NULL;
  }

  char semi_colon = buf_getchar_after_blank(buffer);
  if (semi_colon == ';')
    return ast_new_declaration(ast_new_variable(var_name, 0), right);

  printf("error lors du parsing d\'assignement\n");
  return NULL;
}
ast_t *parse_fncall(buffer_t *buffer, char *fn_name) {
  ast_t *fn_call = malloc(sizeof(ast_t));
  ast_list_t *args = malloc(sizeof(ast_list_t));
  ast_list_t *head = args;
  ast_list_t *cursor = args;
  while (true) {
    ast_t *arg = parse_expr(buffer);
    if (arg == NULL || arg->type == AST_NULL) return NULL;
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
  cursor->node = ast_new_return(NULL);
  cursor->node->type = AST_NULL;
  fn_call = ast_new_fncall(fn_name, head);
  return fn_call;
}

ast_t *parse_arg(buffer_t *buffer) {
  char ch = buf_getchar_after_blank(buffer);
  buf_lock(buffer);
  buf_rollback_and_unlock(buffer, 1);
  if (is_letter(ch) == true) {

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
  ast_list_t *head = malloc(sizeof(ast_list_t));
  ast_list_t *cursor = head;
  ast_list_t *previous = head;

  int len = 0;
  while (true) {
    int rollback_count = buffer->it;
    char *fn_name = lexer_getalphanum(buffer);
    char right_parenthesis = buf_getchar_after_blank(buffer);
    rollback_count = buffer->it - rollback_count;
    if (right_parenthesis == '(' && strlen(fn_name) != 0 &&
        !is_logic_operator(fn_name)) {
      ast_t *fn_call = parse_fncall(buffer, fn_name);
      if (fn_call == NULL) {
        printf("error lors du parsing d\'une fonction");
        return NULL;
      }
      cursor->node = fn_call;
      cursor->next = malloc(sizeof(ast_list_t));
      previous = cursor;
      cursor = cursor->next;
      len++;
      continue;
    }
    buf_lock(buffer);
    buf_rollback_and_unlock(buffer, rollback_count);
    char next_char = buf_getchar_after_blank(buffer);
    if (next_char == ';' || next_char == ',') {
      buf_lock(buffer);
      buf_rollback_and_unlock(buffer, 1);
      break;
    } else if (next_char == '(') {
      ast_t *arg = parse_expr(buffer);
      if (arg == NULL || buf_getchar_after_blank(buffer) != ')') {
        printf("manque une parenthese fermante!\n");
        return NULL;
      }
      ast_t *unary_parenthese = malloc(sizeof(ast_t));
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
    } else if (is_arithmetic_operator(
                   next_char)) {
      if (len % 2 == 0) {
        printf("error lors du parsing d\'une expression\n");
        return NULL;
      }
      ast_binary_e operator= {};
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
    if (is_logic_operator(lexem)) {
      ast_binary_e operator= {};
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
          comp_op = ">";
        }
      } else if (next_char == '=' && buf_getchar(buffer) == '=') {
        comp_op = "==";
      } else {
        printf("error\n");
        return NULL;
      }
      ast_binary_e operator= {};
      operator.op = comp_op;

      ast_t *bin = ast_new_binary(operator, NULL, NULL);
      cursor->node = bin;
      cursor->next = malloc(sizeof(ast_list_t));
      previous = cursor;

      cursor = cursor->next;
      len++;
      continue;
    }
    if (len % 2 != 0 && !is_logic_operator(lexem)) {
      printf("error\n");
      return NULL;
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
  if (len == 0) {
    ast_t *args = malloc(sizeof(ast_t));
    args->type = AST_NULL;
    return args;
  }
  if (len % 2 == 0) {
    printf("nombre insuffisant de membre dans l\'expression\n");
    return NULL;
  }
  previous->next = NULL;
  return NPI(head, len);
}

ast_t *NPI(ast_list_t *expr, int len) {
  if (len == 1) {
    return expr->node;
  }
  if (expr == NULL) {
    return NULL;
  }

  ast_list_t *cursor = expr;
  ast_t *stack[len + 1];
  ast_t *out[len];

  int i = 0;
  int cpt = len - 1;
  stack[0] = NULL;
  while (cursor != NULL && i < len) {
    if (stack[i] == NULL || is_higher_precedence(cursor->node, stack[i])) {
      stack[++i] = cursor->node;
      cursor = cursor->next;
    } else {
      do {
        out[cpt--] = stack[i];
        stack[i--] = NULL;
      } while (stack[i] != NULL &&
               is_higher_precedence(stack[i], out[cpt + 1]));
    }
  }
  while (i > 0) {
    out[cpt--] = stack[i--];
  }
  cpt++;
  int x = 0;
  ast_binary_e e;
  return ast_from_stack(out, ast_new_binary(e, NULL, NULL), &x);
}

ast_t *ast_from_stack(ast_t *stack[], ast_t *tree, int *i) {
  if (stack[*i]->type == AST_VARIABLE || stack[*i]->type == AST_INTEGER ||
      stack[*i]->type == AST_FNCALL ||

      (stack[*i]->type == AST_UNARY && strcmp(stack[*i]->unary.op, "(") == 0)) {
    return stack[*i];
  }

  if (stack[*i]->type == AST_BINARY) {
    if (*i == 0) tree->type = AST_BINARY;
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
  char *left = malloc(sizeof(char) * 3);
  char *right = malloc(sizeof(char) * 3);
  if (a->type == AST_BINARY) {
    left = a->binary.op.op;
  } else if (a->type == AST_INTEGER || a->type == AST_VARIABLE) {
    left = "VAR";

  } else if (a->type == AST_UNARY && strcmp(a->unary.op, "(") == 0) {
    left = "(";
  } else if (a->type == AST_FNCALL) {
    left = "FN_CALL";
  }

  if (b->type == AST_BINARY) {
    right = b->binary.op.op;
  } else if (b->type == AST_INTEGER || b->type == AST_VARIABLE) {
    right = "VAR";
  } else if (b->type == AST_UNARY && strcmp(b->unary.op, "(") == 0) {
    right = "(";
  } else if (b->type == AST_FNCALL) {
    right = "FN_CALL";
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
      if (strcmp(table[i][j], right) == 0) {
        right_rank = TABLE_LEVELS - i;
      }
    }
  }

  return left_rank > right_rank;
}

ast_t *parse_condition(buffer_t *buffer) {
  if (strcmp(lexer_getalphanum(buffer), IF) == 0) {
    char left_parenthesis = buf_getchar_after_blank(buffer);
    if (left_parenthesis != '(') {
      printf("error lors du parsing de condition\n");
      return NULL;
    }
    ast_t *expr = parse_expr(buffer);
    if (expr == NULL) {
      printf("error lors du parsing de condition\n");
      return NULL;
    }
    if (buf_getchar_after_blank(buffer) == ')') {
      if (buf_getchar_after_blank(buffer) != '{') {
        printf("Accolade fermante manquante apres la condition si\n");
        return NULL;
      }
    } else {
      printf("Accolade fermante manquante apres la condition si\n");
      return NULL;
    }

    ast_t *valid_branch = malloc(sizeof(ast_t));

    valid_branch->type = AST_COMPOUND_STATEMENT;
    valid_branch->compound_stmt.stmts = malloc(sizeof(ast_list_t));

    ast_t *invalid_branch = malloc(sizeof(ast_t));

    invalid_branch->type = AST_COMPOUND_STATEMENT;
    invalid_branch->compound_stmt.stmts = malloc(sizeof(ast_list_t));
    invalid_branch->compound_stmt.stmts->node = malloc(sizeof(ast_t));

    invalid_branch->compound_stmt.stmts->node->type = AST_NULL;

    parse_function(buffer, &valid_branch);
    if (valid_branch == NULL) {
      printf("error lors du parsing de condition\n");
      return NULL;
    }
    expr = ast_new_condition(expr, valid_branch, invalid_branch);

    return expr;
  }
  return NULL;
}

ast_t *is_elseif(buffer_t *buffer, ast_t *previous) {
  char *lexem = lexer_getalphanum(buffer);
  char left_parenthesis = buf_getchar_after_blank(buffer);
  if (left_parenthesis != '(') {
    printf("error lors du parsing de condition\n");
    return NULL;
  }
  ast_t *expr = parse_expr(buffer);
  if (expr == NULL) {
    printf("error lors du parsing de condition\n");
    return NULL;
  }
  if (buf_getchar_after_blank(buffer) == ')') {
    if (buf_getchar_after_blank(buffer) != '{') {
      printf("Accolade fermante manquante apres la condition si\n");
      return NULL;
    }
  } else {
    printf("Accolade fermante manquante apres la condition si\n");
    return NULL;
  }

  ast_t *valid_branch = malloc(sizeof(ast_t));

  valid_branch->type = AST_COMPOUND_STATEMENT;
  valid_branch->compound_stmt.stmts = malloc(sizeof(ast_list_t));

  ast_t *invalid_branch = malloc(sizeof(ast_t));

  invalid_branch->type = AST_COMPOUND_STATEMENT;
  invalid_branch->compound_stmt.stmts = malloc(sizeof(ast_list_t));
  invalid_branch->compound_stmt.stmts->node = malloc(sizeof(ast_t));

  invalid_branch->compound_stmt.stmts->node->type = AST_NULL;

  parse_function(buffer, &valid_branch);
  if (valid_branch == NULL) {
    printf("error lors du parsing de condition\n");
    return NULL;
  }
  expr = ast_new_condition(expr, valid_branch, invalid_branch);
  previous->branch.invalid = expr;
  return expr;
}

ast_t *parse_loop(buffer_t *buffer) {
  if (strcmp(lexer_getalphanum(buffer), "tantque") == 0) {
    if (buf_getchar_after_blank(buffer) != '(') {
      printf("parenthese ouvrante manquante au debut d\'une tantque\n");
      return NULL;
    }
    ast_t *expr = parse_expr(buffer);
    if (expr == NULL) {
      printf("erreur lors du parsing d\'une boucle tantque\n");
      return NULL;
    }
    ast_t *loop = ast_new_loop(expr, malloc(sizeof(ast_list_t)));
    if (buf_getchar_after_blank(buffer) == ')') {
      if (buf_getchar_after_blank(buffer) != '{') {
        printf("Accolade fermante manquante apres la condition si\n");
        return NULL;
      }
    } else {
      printf("Accolade fermante manquante apres la condition si\n");
      return NULL;
    }
    parse_function(buffer, &loop);
    if (loop == NULL) {
      printf("error lors du parsing de loop\n");
      return NULL;
    }
    return loop;
  }
  return NULL;
}

ast_t *is_fncall(buffer_t *buffer) {
  char *fn_call_name = lexer_getalphanum(buffer);
  if (buf_getchar_after_blank(buffer) == '(' && strlen(fn_call_name) != 0) {
    ast_t *expr = parse_fncall(buffer, fn_call_name);
    if (expr == NULL) {
      printf("error dans un appel un fonction\n");
      return NULL;
    }
    if (buf_getchar_after_blank(buffer) != ';') {
      printf("point virgule manquant\n");
      return NULL;
    }

    if (expr == NULL) {
      return NULL;
    }
    return expr;
  }
  return NULL;
}