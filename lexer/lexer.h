#include "../buffer/buffer.h"

char * lexer_getalphanum (buffer_t * buffer);
char * lexer_getalphanum_rollback(buffer_t * buffer);
long lexer_getnumber (buffer_t * buffer);