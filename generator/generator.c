#include "generator.h"
#include <string.h>

void generate_code(ast_list_t* tree, FILE *file) {
    ast_list_t *cursor = tree;

    while (cursor->node->type != AST_NULL) {
        ast_t *ast = cursor->node; 
        if (ast->type == AST_FUNCTION) {
            char *func_name = ast->function.name;
            ast_list_t *params = ast->function.params;
            ast_list_t *cursor_params = params;

            fputs("function ", file);

            fputs(func_name, file);
            fputs("(",file);

            while (cursor_params->node->type != AST_NULL) {
                char *param_name = cursor_params->node->var.name;
                fputs(param_name, file);
                if (cursor_params->next->node->type == AST_NULL) {
                    break;
                }
                fputs(", ", file);
                cursor_params = cursor_params->next;
            }
            fputs(")", file);
            fputs( "{", file);
            fputs("\n", file);
            generate_stmts(ast->function.stmts, file, 1);
            fputs("}\n\n", file);
        }

        cursor = cursor->next;
    }
}

void generate_stmts(ast_list_t* stmts, FILE *file, int indent_level) {
    ast_list_t *cursor = stmts;

    while (cursor->node->type != AST_NULL) {
        ast_t *ast = cursor->node; 
        indent(file, indent_level);
        if (ast->type == AST_DECLARATION) {
            char *var_name = ast->declaration.lvalue->var.name;
            char *value = build_expr(ast->declaration.rvalue);
            fputs("let ", file);
            fputs(var_name, file);
            if (ast->declaration.rvalue != NULL) {
                fputs(" = ", file);
                fputs(value, file);
            }
            fputs(";\n", file);

        } else if (ast->type == AST_ASSIGNMENT) {
            char *var_name = ast->assignment.lvalue->var.name;
            char *value = build_expr(ast->assignment.rvalue);
            fputs(var_name, file);
            fputs(" = ", file);
            fputs(value, file);
            fputs(";\n", file);
        } else if (ast->type == AST_CONDITION) {
            fputs("if (", file);
            fputs(build_expr(ast->branch.condition), file);            
            fputs(") {\n", file);
                generate_stmts(ast->branch.valid->compound_stmt.stmts, file, indent_level+1);

            indent(file, indent_level);
    
            fputs("}\n", file);
            ast_t *cursor_branch = ast->branch.invalid;
  
            while (cursor_branch->type == AST_CONDITION) {
                indent(file, indent_level);
                      
                fputs("else if (", file);
                fputs(build_expr(cursor_branch->branch.condition), file);
                fputs(") {\n", file);

                generate_stmts(cursor_branch->branch.valid->compound_stmt.stmts, file, indent_level+1);

                indent(file, indent_level);
                fputs("}\n", file);

                cursor_branch = cursor_branch->branch.invalid;
            }
            if (cursor_branch->type == AST_COMPOUND_STATEMENT && cursor_branch->compound_stmt.stmts->node->type != AST_NULL) {
                indent(file, indent_level);
                fputs("else {\n", file);
                generate_stmts(cursor_branch->compound_stmt.stmts, file, indent_level+1);
                indent(file, indent_level);
                fputs("}\n", file);
            }        

        } else if (ast->type == AST_LOOP) {
            fputs("while (", file);
            char *loop = build_expr(ast->loop.condition);
            fputs(loop, file);
            fputs(") {\n", file);

            generate_stmts(ast->loop.stmts, file, indent_level+1);
            indent(file, indent_level);
            fputs("}\n", file);
        } else if (ast->type == AST_FNCALL) {
            char *func_name = ast->call.name;
            fputs(func_name, file);
            fputs("(", file);
            ast_list_t *cursor_args = ast->call.args;

            while (true) {
                fputs(build_expr(cursor_args->node), file);
                cursor_args = cursor_args->next;
                if (cursor_args->node->type == AST_NULL) {
                    break;
                }
                fputs(", ", file);

            }
            fputs(");\n", file);

        } else if (ast->type == AST_RETURN) {
            fputs("return ", file);
            fputs(build_expr(ast->ret.expr), file);
            fputs(";\n", file);
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

void indent(FILE *file,int indent_level) {
    for (int i = 0; i < indent_level; i++) {
        fputs("\t", file);
    }
}