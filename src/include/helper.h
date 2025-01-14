#ifndef __HELPER_H__
#define __HELPER_H__

#include "lexer.h"
#include "parser.h"

void print_token(token_T *token);
const char *ast_type_as_string(ast_type_T at);
void print_ast_as_tree(ast_T *ast);

#endif // __HELPER_H__
