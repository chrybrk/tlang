#ifndef __parser_h__
#define __parser_h__

#include "glob.h"
#include "lexer.h"

typedef struct AST_STRUCT ast_T;

typedef struct AST_STRUCT {
	ast_type_T type;
	data_type_T data_type;
	token_T *token;
	ast_T *left;
	ast_T *mid;
	ast_T *right;
	size_t index;
} ast_T;

typedef struct {
	list_T *tokens;
	token_T *token;
	ssize_t index;
} parser_T;

ast_T *init_ast(
	ast_type_T type, data_type_T data_type, token_T *token,
	ast_T *left, ast_T *mid, ast_T *right, size_t index
);
ast_T *init_ast_leaf(ast_type_T type, data_type_T data_type, token_T *token, size_t index);
ast_T *init_ast_unary(ast_type_T type, data_type_T data_type, token_T *token, ast_T *left, size_t index);

void pretty_ast_tree(ast_T *root, int level);

parser_T *init_parser(list_T *tokens);
ast_T *parser_parse(parser_T *parser);

#endif // __parser_h__
