#include "generator.h"

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

            while (true) {
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
            generate_stmts(ast, file);
        }

        cursor = cursor->next;
    }
        fputs("\n}", file);
        fclose(file);
        return true;
}

bool generate_stmts(ast_t* function, FILE *file) {
    ast_list_t *cursor = function->function.stmts;

    while (cursor->node->type != AST_NULL) {
        ast_t *ast = cursor->node; 
            fputs("\t", file);

        if (ast->type == AST_DECLARATION) {
            char *var_name = ast->assignment.lvalue->var.name;
            char *value = malloc(sizeof(char) * 10);
            sprintf(value, "%d", ast->assignment.rvalue->integer);
            fputs("let ", file);
            fputs(var_name, file);
            fputs(" = ", file);
            fputs(value, file);
            fputs(";", file);

        }
        cursor = cursor->next;
    }
}