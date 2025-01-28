#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "include/parser.h"
#include "include/helper.h"
#include "include/global.h"

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

void create_log(parser_T *parser, LOG_KIND kind, const char *log)
{
	const char *color_log;
	switch (kind)
	{
		case ERROR:
			color_log =
				formate_string("%serror%s", get_term_color(BOLD_TEXT, RED), get_term_color(RESET, WHITE));
			break;

		case WARN:
			color_log =
				formate_string("%swarn%s", get_term_color(BOLD_TEXT, YELLOW), get_term_color(RESET, WHITE));
			break;

		case INFO:
			color_log =
				formate_string("%sinfo%s", get_term_color(BOLD_TEXT, GREEN), get_term_color(RESET, WHITE));
			break;
	}

	error_flush(
			kind,
			formate_string("%s:%ld:%ld: %s: %s\n\t %ld | %s\n",
					filename,
					parser->token->position.line, parser->token->position.column,
					color_log,
					log,
					parser->token->position.line, get_current_line(parser->lexer)

			)
	);
}

token_T *parser_eat(parser_T *parser, int type)
{
	if (type >= 0 && parser->token->type != (token_type_T)type)
		create_log(parser, ERROR, formate_string("expected `%s` but got `%s`",
					token_type_as_string(type), token_type_as_string(parser->token->type)
		));

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
	switch (parser->token->type)
	{
		case TT_INT:
		case TT_ID:
			return init_ast_left(AST_PRIMARY, parser_eat(parser, -1), NULL);
		default:
			return NULL;
	}
}

ast_T *parser_parse_factor(parser_T *parser)
{
	ast_T *left = parser_parse_primary(parser);

	while (match_token(parser->token, TT_LP))
	{
		parser_eat(parser, TT_LP);
		ast_T *right = parser_parse_expr(parser);
		parser_eat(parser, TT_RP);

		if (!right)
		{
			create_log(parser, ERROR, "expected `expr` here");
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

		if (!right)
		{
			create_log(parser, ERROR, "expected `factor` after `*, /`, did you forgot to add it?");
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

		if (!right)
		{
			create_log(parser, ERROR, "expected `term` after `+, -`, did you forgot to add it?");
			return NULL;
		}

		left = init_ast_lr(AST_EXPR, token, left, right);
	}

	return left;
}

// FIXME: line:column are not at right position, because when we create log,
// it is capturing the current position, which might be off from where i want it.
//
// TODO: create_log should take position structure, so that it knows where to point at.
ast_T *parser_parse_let(parser_T *parser)
{
	ast_T *ast = NULL;

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

	if (!left || match_token_with_data_type(left->token) < 0)
		create_log(parser, ERROR, "expected `(i8, i16, i32, i64, char, string)` after `:`");

	ast_T *right = NULL;
	if (peek_token(parser, 0)->type == TT_ASSIGN)
	{
		// assignment operator
		parser_eat(parser, TT_ASSIGN);

		right = parser_parse_expr(parser);

		if (!right)
		{
			create_log(parser, ERROR, "maybe you meant to do `let <name>: <type>;`");
			create_log(parser, ERROR, "OR maybe you meant to do `let <name>: <type> = <expr>;`");
		}
	}

	ast = right ?
		init_ast_lr(AST_LET, var_name->type == TT_EOF ? NULL : var_name, left, right) :
		init_ast_left(AST_LET, var_name->type == TT_EOF ? NULL : var_name, left);

	return ast;
}

ast_T *parser_parse_return(parser_T *parser)
{
	parser_eat(parser, -1);

	ast_T *left = parser_parse_expr(parser);
	if (!left)
		create_log(parser, ERROR, "return expects expr");

	return init_ast_left(AST_RETURN, NULL, left);
}

ast_T *parser_parse_id(parser_T *parser)
{
	if (!strcmp(parser->token->value, "let"))
		return parser_parse_let(parser);

	else if (!strcmp(parser->token->value, "ret"))
		return parser_parse_return(parser);

	return parser_parse_expr(parser);
}

ast_T *parser_parse_assignment(parser_T *parser)
{
	token_T *name = parser_eat(parser, TT_ID);

	parser_eat(parser, TT_ASSIGN);

	ast_T *expr = parser_parse_expr(parser);

	return init_ast_left(AST_ASSIGN, name, expr);
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
				case TT_ASSIGN: ast = parser_parse_assignment(parser); break;
				default: ast = parser_parse_id(parser);
			}
			break;
		}
		default: ast = parser_parse_expr(parser);
	}

	if (ast)
		parser_eat(parser, TT_SEMI);

	return ast;
}

ast_T *parser_parse(parser_T *parser)
{
	ast_T *program = parser_parse_statement(parser);

	while (program && parser->token->type != TT_EOF)
		program = init_ast_lr(AST_STMT, NULL, program, parser_parse_statement(parser));

	if (!program)
	{
		const char *log = formate_string(
				"%s:%ld:%ld: %swarn%s: expected `let`, `extern`, `fn`, ... but got empty file.",
				filename,
				parser->lexer->position.line, parser->lexer->position.column,
				get_term_color(BOLD_TEXT, YELLOW), get_term_color(RESET, WHITE)
		);
		error_flush(WARN, log);
	}

	return program;
}
