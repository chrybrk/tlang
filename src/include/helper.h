#ifndef __HELPER_H__
#define __HELPER_H__

#include <ctype.h>
#include "lexer.h"
#include "parser.h"

const char *token_type_as_string(token_type_T at);
void print_token(token_T *token);
const char *ast_type_as_string(ast_type_T at);
void print_ast_as_tree(ast_T *ast, int level);
int isnotdq(int c);
int isident(int c);

#endif // __HELPER_H__
