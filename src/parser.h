#ifndef __parser_h__
#define __parser_h__

#include <stdbool.h>
#include "lex.h"
#include "glob.h"

typedef enum {
	ast_program,
	ast_statement,
	ast_expr,
	ast_term,
	ast_return,
	ast_var,
	ast_assign,
	ast_data_type,
	ast_class_type,
	ast_class_call,
	ast_function,
	ast_call,
	ast_ident,
	ast_const,
	ast_join,
	ast_noop
} ast_type_T;

typedef struct AST_STRUCT ast_T;

typedef struct AST_STRUCT {
	ast_type_T type;
	token_T *token;
	data_type_T data_type;
	ast_T *left;
	ast_T *mid;
	ast_T *right;
} ast_T;

typedef struct {
	list_T *tokens;
	token_T *token;
	ssize_t index;
} parser_T;

ast_T *init_ast(ast_type_T type, token_T *token, ast_T *left, ast_T *mid, ast_T *right);
ast_T *init_ast_dt(ast_type_T type, token_T *token, data_type_T data_type, ast_T *left, ast_T *mid, ast_T *right);
ast_T *init_ast_jt(ast_type_T type);
ast_T *init_ast_tt(ast_type_T type, token_T *token);
ast_T *init_ast_l(ast_type_T type, token_T *token, ast_T *left);
ast_T *init_ast_lr(ast_type_T type, token_T *token, ast_T *left, ast_T *right);
ast_T *init_ast_m(ast_type_T type, token_T *token, ast_T *mid);
ast_T *init_ast_tt_dt(ast_type_T type, token_T *token, data_type_T data_type);
ast_T *init_ast_l_dt(ast_type_T type, token_T *token, data_type_T data_type, ast_T *left);
ast_T *init_ast_lr_dt(ast_type_T type, token_T *token, data_type_T data_type, ast_T *left, ast_T *right);
ast_T *init_ast_m_dt(ast_type_T type, token_T *token, data_type_T data_type, ast_T *mid);

void pretty_ast_tree(ast_T *root, int level);

parser_T *init_parser(list_T *tokens);
ast_T *parser_parse_factor(parser_T *parser);
ast_T *parser_parse_term(parser_T *parser);
ast_T *parser_parse_expr(parser_T *parser);
ast_T *parser_parse_statement(parser_T *parser);
ast_T *parser_parse_compound_statement(parser_T *parser);
ast_T *parser_parse_program(parser_T *parser);

#endif // __parser_h__
