#include <string.h>
#include "include/parser.h"
#include "include/helper.h"

ast_T *init_ast(ast_type_T type, token_T *token, ast_T *left, ast_T *mid, ast_T *right)
{
	ast_T *ptr = malloc(sizeof(struct AST_STRUCT));
	if (ptr == NULL)
	{
		perror("failed to allocate memory for ast: ");
		return NULL;
	}
	
	ptr->type = type;
	ptr->token = token;
	ptr->left = left;
	ptr->mid = mid;
	ptr->right = right;

	return ptr;
}

ast_T *init_ast_with_type(ast_type_T type)
{
	return init_ast(type, NULL, NULL, NULL, NULL);
}

ast_T *init_ast_left(ast_type_T type, token_T *token, ast_T *left)
{
	return init_ast(type, token, left, NULL, NULL);
}

ast_T *init_ast_lm(ast_type_T type, token_T *token, ast_T *left, ast_T *mid)
{
	return init_ast(type, token, left, mid, NULL);
}

ast_T *init_ast_lr(ast_type_T type, token_T *token, ast_T *left, ast_T *right)
{
	return init_ast(type, token, left, NULL, right);
}

parser_T *init_parser(lexer_T *lexer)
{
	parser_T *ptr = malloc(sizeof(struct PARSER_STRUCT));
	if (ptr == NULL)
	{
		perror("failed to allocate memory for parser: ");
		return NULL;
	}

	ptr->lexer = lexer;
	ptr->token = NULL;

	return ptr;
}

token_T *parser_eat(parser_T *parser)
{
	parser->token = next_token(parser->lexer);
	return parser->token;
}

token_T *peek_token(parser_T *parser, unsigned int offset)
{
	lexer_T *lexer_cp = init_lexer(parser->lexer->source);
	memcpy(lexer_cp, parser->lexer, sizeof(struct LEXER_STRUCT));

	token_T *token;
	for (unsigned int i = 0; i <= offset; ++i)
		token = next_token(lexer_cp);

	free(lexer_cp);

	return token;
}

int match_token(token_T *token, token_type_T type)
{
	return token && token->type == type ? 1 : 0;
}

ast_T *parser_parse_number(parser_T *parser)
{
	token_T *token = parser_eat(parser);
	int is_token_number = match_token(token, TT_INT);

	if (is_token_number) return init_ast_left(AST_NUMBER, token, NULL);

	return NULL;
}

ast_T *parser_parse_factor(parser_T *parser)
{
	ast_T *left = parser_parse_number(parser);
	return left ? init_ast(AST_FACTOR, NULL, left, NULL, NULL) : NULL;
}

ast_T *parser_parse_term(parser_T *parser)
{
	ast_T *left = parser_parse_factor(parser);

	token_T *token;

	while ((token = peek_token(parser, 0)) && (match_token(token, TT_STAR) || match_token(token, TT_FSLASH)))
	{
		token = parser_eat(parser);
		ast_T *right = parser_parse_factor(parser);

		if (right == NULL)
		{
			printf("failed to parse right node of term.\n");
			return NULL;
		}

		left = init_ast_lr(AST_TERM, token, left, right);
	}

	return left ? left : NULL;
}

ast_T *parser_parse_expr(parser_T *parser)
{
	ast_T *left = parser_parse_term(parser);

	token_T *token;

	while ((token = peek_token(parser, 0)) && (match_token(token, TT_PLUS) || match_token(token, TT_MINUS)))
	{
		token = parser_eat(parser);
		ast_T *right = parser_parse_term(parser);

		if (right == NULL)
		{
			printf("failed to parse right node of expr.\n");
			return NULL;
		}

		left = init_ast_lr(AST_EXPR, token, left, right);
	}

	return left ? left : NULL;
}

ast_T *parser_parse(parser_T *parser)
{
	ast_T *program = init_ast_with_type(AST_PRGM);
	program->left = parser_parse_expr(parser);

	/*
	 * left -> statement -> expr
	 * right -> statement
	 * 						left -> statement -> expr
	 * 						right -> statement
	 * 											-> left -> 
	*/

	return program;
}
