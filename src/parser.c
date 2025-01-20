#include <stdio.h>
#include <string.h>
#include <stdbool.h>
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
	ptr->token = next_token(ptr->lexer);

	return ptr;
}

token_T *parser_eat(parser_T *parser, int type)
{
	if (type >= 0 && parser->token->type != (token_type_T)type)
	{
		fprintf(stderr, "expected `%s` got `%s`.\n", token_type_as_string(type), token_type_as_string(parser->token->type));
		exit(1);
	}

	token_T *token = parser->token;
	parser->token = next_token(parser->lexer);

	return token;
}

token_T *peek_token(parser_T *parser, unsigned int offset)
{
	lexer_T *lexer_cp = init_lexer(parser->lexer->source);
	memcpy(lexer_cp, parser->lexer, sizeof(struct LEXER_STRUCT));

	token_T *token = parser->token;
	for (unsigned int i = 0; i < offset; ++i)
		token = next_token(lexer_cp);

	free(lexer_cp);

	return token;
}

int match_token(token_T *token, token_type_T type)
{
	return token && token->type == type ? 1 : 0;
}

int match_token_with_data_type(token_T *token)
{
	if (!token) return -1;

	const char *data_type[] = {
		"i8",
		"i16",
		"i32",
		"i64",
		"char",
		"string",
		"void"
	};

	size_t index = 0;
	size_t size = sizeof(data_type) / sizeof(data_type[0]);
	for (; index < size; ++index)
	{
		if (!strcmp(token->value, data_type[index]))
			return index;
	}

	return -1;
}

ast_T *parser_parse_primary(parser_T *parser)
{
	if (!match_token(parser->token, TT_INT) && !match_token(parser->token, TT_ID))
		return NULL;

	token_T *token = parser_eat(parser, -1);
	return init_ast_left(AST_PRIMARY, token, NULL);
}

ast_T *parser_parse_factor(parser_T *parser)
{
	ast_T *left = parser_parse_primary(parser);

	while (match_token(parser->token, TT_LP))
	{
		parser_eat(parser, TT_LP);
		ast_T *right = parser_parse_expr(parser);
		parser_eat(parser, TT_RP);

		if (right == NULL)
		{
			printf("failed to parse right node of factor.\n");
			return NULL;
		}

		left = init_ast_lr(AST_FACTOR, NULL, left, right);
	}

	return left ? init_ast(AST_FACTOR, NULL, left, NULL, NULL) : NULL;
}

ast_T *parser_parse_term(parser_T *parser)
{
	ast_T *left = parser_parse_factor(parser);

	token_T *token;
	while ((token = peek_token(parser, 0)) && (match_token(token, TT_STAR) || match_token(token, TT_FSLASH)))
	{
		token = parser_eat(parser, -1);
		ast_T *right = parser_parse_factor(parser);

		if (right == NULL)
		{
			printf("failed to parse right node of term.\n");
			return NULL;
		}

		left = init_ast_lr(AST_TERM, token, left, right);
	}

	return left;
}

ast_T *parser_parse_expr(parser_T *parser)
{
	ast_T *left = parser_parse_term(parser);

	token_T *token;
	while ((token = peek_token(parser, 0)) && (match_token(token, TT_PLUS) || match_token(token, TT_MINUS)))
	{
		token = parser_eat(parser, -1);

		ast_T *right = parser_parse_term(parser);

		if (right == NULL)
		{
			printf("failed to parse right node of expr.\n");
			return NULL;
		}

		left = init_ast_lr(AST_EXPR, token, left, right);
	}

	return left;
}

ast_T *parser_parse_let(parser_T *parser)
{
	// eat `let`
	parser_eat(parser, -1);

	// now we should follow this:
	// 	let var_name: type = expr

	// variable name
	token_T *var_name = parser_eat(parser, TT_ID);

	// colon
	parser_eat(parser, TT_COLON);

	// type of variable
	ast_T *left = parser_parse_primary(parser);

	int is_data_type_valid = match_token_with_data_type(left->token);
	if (is_data_type_valid < 0)
		fprintf(stderr,
				"expected data type are `i8, i16, i32, i64, char, string, void`, got `%s`.\n",
				left->token->value),
		exit(1);

	// assignment operator
	parser_eat(parser, TT_ASSIGN);

	return init_ast_lr(AST_LET, var_name, left, parser_parse_expr(parser));
}

ast_T *parser_parse_id(parser_T *parser)
{
	if (!strcmp(parser->token->value, "let"))
		return parser_parse_let(parser);

	return parser_parse_expr(parser);
}

ast_T *parser_parse_call(parser_T *parser)
{
	// function name
	token_T *name = parser_eat(parser, TT_ID);

	parser_eat(parser, TT_LP);

	ast_T *expr = parser_parse_expr(parser);

	parser_eat(parser, TT_RP);

	return init_ast_left(AST_CALL, name, expr);
}

ast_T *parser_parse_statement(parser_T *parser)
{
	ast_T *ast = NULL;

	switch (parser->token->type)
	{
		case TT_ID:
		{
			switch (peek_token(parser, 1)->type)
			{
				case TT_LP: ast = parser_parse_call(parser); break;
				default: ast = parser_parse_let(parser);
			}
			break;
		}
		default: ast = parser_parse_expr(parser);
	}

	parser_eat(parser, TT_SEMI);

	return ast;
}

ast_T *parser_parse(parser_T *parser)
{
	ast_T *program = parser_parse_statement(parser);

	while (parser->token->type != TT_EOF)
		program = init_ast_lr(AST_STMT, NULL, program, parser_parse_statement(parser));

	return program;
}
