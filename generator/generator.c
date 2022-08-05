#include "generator.h"
#include <string.h>
char *output;
char * generate_code(ast_list_t* tree) {
    output = malloc(sizeof(char) * 20048);    
    ast_list_t *cursor = tree;

    strcpy(output, "");
    while (cursor->node->type != AST_NULL) {
        ast_t *ast = cursor->node; 
        if (ast->type == AST_FUNCTION) {
            char *func_name = ast->function.name;
            ast_list_t *params = ast->function.params;
            ast_list_t *cursor_params = params;

            strcat(output, "function ");

            strcat(output, func_name);

            strcat(output, "(");

            while (cursor_params->node->type != AST_NULL) {
                char *param_name = cursor_params->node->var.name;
                strcat(output, param_name);

                if (cursor_params->next->node->type == AST_NULL) {
                    break;
                }
                strcat(output, ", ");

                cursor_params = cursor_params->next;
            }
            strcat(output, ")");
            strcat(output, "{");
            strcat(output, "\n");

          
            generate_stmts(ast->function.stmts, 1);
            strcat(output, "}\n\n");

        }

        cursor = cursor->next;
    }
    return output;
}

void generate_stmts(ast_list_t* stmts,  int indent_level) {
    ast_list_t *cursor = stmts;

    while (cursor->node->type != AST_NULL) {
        ast_t *ast = cursor->node; 
        indent(indent_level);
        if (ast->type == AST_DECLARATION) {
            char *var_name = ast->declaration.lvalue->var.name;
            char *value = build_expr(ast->declaration.rvalue);
   
            strcat(output, "let ");
            strcat(output, var_name);

            if (ast->declaration.rvalue != NULL) {

                strcat(output, " = ");
                strcat(output, value);


            }
            strcat(output, ";\n");

        } else if (ast->type == AST_ASSIGNMENT) {
            char *var_name = ast->assignment.lvalue->var.name;
            char *value = build_expr(ast->assignment.rvalue);
            strcat(output, var_name );

            strcat(output, " = ");


            strcat(output, value);

            strcat(output, ";\n");

        } else if (ast->type == AST_CONDITION) {
            strcat(output, "if (");
            strcat(output, build_expr(ast->branch.condition));
            strcat(output, ") {\n");
         
            generate_stmts(ast->branch.valid->compound_stmt.stmts, indent_level+1);

            indent( indent_level);
    
            strcat(output,"}\n");

            ast_t *cursor_branch = ast->branch.invalid;
  
            while (cursor_branch->type == AST_CONDITION) {
                indent( indent_level);
                      
                strcat(output, "else if (");

                strcat(output, build_expr(cursor_branch->branch.condition));

                strcat(output, ") {\n");

                generate_stmts(cursor_branch->branch.valid->compound_stmt.stmts, indent_level+1);

                indent( indent_level);
                strcat(output, "}\n");

                cursor_branch = cursor_branch->branch.invalid;
            }
            if (cursor_branch->type == AST_COMPOUND_STATEMENT && cursor_branch->compound_stmt.stmts->node->type != AST_NULL) {
                indent( indent_level);
                strcat(output, "else {\n");

                generate_stmts(cursor_branch->compound_stmt.stmts, indent_level+1);
                indent( indent_level);
                strcat(output, "}\n");

            }        

        } else if (ast->type == AST_LOOP) {
                strcat(output, "while (");

            char *loop = build_expr(ast->loop.condition);
                strcat(output, loop);

                strcat(output, ") {\n");

            generate_stmts(ast->loop.stmts, indent_level+1);
            indent( indent_level);
                strcat(output, "}\n");

        } else if (ast->type == AST_FNCALL) {
            char *func_name = ast->call.name;
                strcat(output, func_name);
                strcat(output, "(");

            ast_list_t *cursor_args = ast->call.args;

            while (true) {
                strcat(output, build_expr(cursor_args->node));

                cursor_args = cursor_args->next;
                if (cursor_args->node->type == AST_NULL) {
                    break;
                }
                strcat(output, ", ");

            }
                strcat(output, ");\n");

        } else if (ast->type == AST_RETURN) {

                strcat(output, "return ");
                strcat(output, build_expr(ast->ret.expr));
                strcat(output, ";\n");

        }
        cursor = cursor->next;
    }
}

char* convert_operator(char *op) {
    if (strcmp(op, "ET") == 0)
        return "&&";
    if (strcmp(op, "OU") == 0)
        return "||"; 
    return op;
}

char* build_expr(ast_t* expr) {
    if (expr == NULL) {
        return "";
    }
    if (expr->type == AST_FNCALL) {
        ast_list_t *cursor_args = expr->call.args;
        int len = 0;
        while (cursor_args->node->type != AST_NULL) {
            len += strlen(build_expr(cursor_args->node)) +1;
            cursor_args = cursor_args->next;
        }
        char *str = malloc(sizeof(char) * len);
        strcpy(str, "");
        cursor_args = expr->call.args;
        while (true) {
            strcat(str,build_expr(cursor_args->node));
            if (cursor_args->next->node->type == AST_NULL) {
                break;
            }
            strcat(str, ",");
            cursor_args = cursor_args->next;
        }

        str[len] = '\0';
        char *total = malloc(sizeof(char) * (strlen(str) + (strlen(expr->call.name) + 3)));
        strcpy(total, expr->call.name);
        strcat(total, "(");
        strcat(total, str);
        strcat(total, ")");
        return total;
    }
    if (expr->type == AST_VARIABLE) {
        return expr->var.name;
    }
    if (expr->type == AST_INTEGER) {
        char *str = malloc(sizeof(char) * 11); 
        sprintf(str, "%ld", expr->integer);
        return str;
    }    
    if (expr->type == AST_BINARY) {
        char *op = malloc(sizeof(char) *10);
        strcpy( op, " " );
        strcat( op, convert_operator(expr->binary.op.op) );
        strcat( op, " " );

        char *right_expr = build_expr(expr->binary.right);
        char *right = malloc(sizeof(char) * (strlen(right_expr) + 1));

        char *left_expr = build_expr(expr->binary.left);
        char *left = malloc(sizeof(char) * (strlen(left_expr) + strlen(op) + 1));

        strcpy(right, right_expr);
        strcpy(left,left_expr);
        strcat(left,op);

        char* total = malloc(sizeof(char) * (strlen(left) + strlen(right) + 1));
        strcpy(total, left);
        strcat(total, right);
        free(op);
        free(right);
        free(left);
        return total;
    }
    if (expr->type == AST_UNARY) {
        char *str = build_expr(expr->unary.operand);
        char *cpy = malloc(sizeof(char) * (strlen(str) + 3));
        strcpy( cpy, "(" );
        strcat( cpy,str );
        strcat( cpy, ")" );
        free(str);
        return cpy;
    }
    return "";
}

void indent(int indent_level) {
    for (int i = 0; i < indent_level; i++) {
        strcat(output, "\t");

    }
}