#ifndef __PARSER_H__
#define __PARSER_H__

#include "lexer.h"
#include "global.h"

typedef enum
{
	AST_LET,
	AST_ASSIGN,
	AST_CALL,
	AST_PRIMARY,
	AST_FACTOR,
	AST_TERM,
	AST_EXPR,
	AST_LIST,
	AST_STMT,
	AST_PRGM
} ast_type_T;

typedef enum
{
	d_i8,
	d_i16,
	d_i32,
	d_i64,
	d_char,
	d_string,
	d_void
} data_type_T;

typedef struct AST_STRUCT ast_T;

typedef struct AST_STRUCT
{
	ast_type_T type;
	token_T *token;
	ast_T *left;
	ast_T *mid;
	ast_T *right;
} ast_T;

typedef struct PARSER_STRUCT
{
	lexer_T *lexer;
	token_T *token;
} parser_T;

ast_T *init_ast(ast_type_T type, token_T *token, ast_T *left, ast_T *mid, ast_T *right);
ast_T *init_ast_with_type(ast_type_T type);
ast_T *init_ast_left(ast_type_T type, token_T *token, ast_T *left);
ast_T *init_ast_lm(ast_type_T type, token_T *token, ast_T *left, ast_T *mid);
ast_T *init_ast_lr(ast_type_T type, token_T *token, ast_T *left, ast_T *right);

parser_T *init_parser(lexer_T *lexer);
ast_T *parser_parse_expr(parser_T *parser);
ast_T *parser_parse(parser_T *parser);

#endif // __PARSER_H__
