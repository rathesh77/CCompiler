#include "generator.h"
#include <string.h>

bool generate_code(ast_list_t* tree) {
    FILE * file;
    file = fopen("output/sample.js", "w");
    if(file == NULL)
    {
        printf("Unable to create file.\n");
        return false;
    }    
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
        }

        cursor = cursor->next;
    }
        fputs("\n}", file);
        fclose(file);
        return true;
}

bool generate_stmts(ast_list_t* stmts, FILE *file, int indent_level) {
    ast_list_t *cursor = stmts;

    while (cursor->node->type != AST_NULL) {
        ast_t *ast = cursor->node; 
        for (int i = 0; i < indent_level; i++) {
            fputs("\t", file);
        }
        if (ast->type == AST_DECLARATION) {
            char *var_name = ast->declaration.lvalue->var.name;
            char *value = build_expr(ast->declaration.rvalue);
            fputs("let ", file);
            fputs(var_name, file);
            fputs(" = ", file);
            fputs(value, file);
            fputs(";\n", file);

        } else if (ast->type == AST_CONDITION) {
            fputs("if (", file);
            char *condition = build_expr(ast->branch.condition);
            fputs(condition, file);
            fputs(") {\n", file);
            for (int i = 0; i < indent_level; i++) {
                fputs("\t", file);
            }            
            fputs("}\n", file);

        } else if (ast->type == AST_LOOP) {
            fputs("while (", file);
            char *loop = build_expr(ast->loop.condition);
            fputs(loop, file);
            fputs(") {\n", file);
            for (int i = 0; i < indent_level; i++) {
                fputs("\t", file);
            }
            generate_stmts(ast->loop.stmts, file, indent_level+1);
            fputs("}\n", file);
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
    if (expr->type == AST_VARIABLE) {
        return expr->var.name;
    }
    if (expr->type == AST_INTEGER) {
        char *str = malloc(sizeof(char) * 11); 
        sprintf(str, "%d", expr->integer);
        return str;
    }    
    if (expr->type == AST_BINARY) {
              char op[10];
        strcpy( op, " " );
        strcat( op, convert_operator(expr->binary.op.op) );
        strcat( op, " " );

        
        char *right_expr = build_expr(expr->binary.right);
        char *right = malloc(sizeof(char) * strlen(right_expr));

        char *left_expr = build_expr(expr->binary.left);

        char *left = malloc(sizeof(char) * strlen(left_expr) + strlen(op));

        strcpy(right, right_expr);
        strcpy(left,left_expr);
        strcat(left,op);

        char* total = malloc(sizeof(char) * (strlen(left) + strlen(right)));
        strcpy(total, left);
        strcat(total, right);

        return total;}
    if (expr->type == AST_UNARY) {
        char *str = build_expr(expr->unary.operand);
        char *cpy = malloc(sizeof(char) * 255);
        strcpy( cpy, "(" );
        strcat( cpy,str );
        strcat( cpy, ")" );
        return &(cpy[0]);
    }
}

char *concat_expr(char *a, char *b) {

    return strcat(a,b);
}