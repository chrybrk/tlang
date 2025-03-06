#include "parser.h"

ast_T *init_ast(
	ast_type_T type, data_type_T data_type, token_T *token,
	ast_T *left, ast_T *mid, ast_T *right, size_t index)
{
	ast_T *ast = malloc(sizeof(struct AST_STRUCT));
	if (!ast)
	{
		printf("err :: init_ast_dt :: failed to allocate memory.\n");
		return NULL;
	}
	ast->type = type;
	ast->token = token;
	ast->data_type = data_type;
	ast->left = left;
	ast->mid = mid;
	ast->right = right;
	ast->index = index;
	return ast;
}

ast_T *init_ast_leaf(ast_type_T type, data_type_T data_type, token_T *token, size_t index)
{
	return init_ast(type, data_type, token, NULL, NULL, NULL, index);
}

ast_T *init_ast_unary(ast_type_T type, data_type_T data_type, token_T *token, ast_T *left, size_t index)
{
	return init_ast(type, data_type, token, left, NULL, NULL, index);
}

const char *ast_type_as_string(ast_type_T type)
{
	char *v;

	switch (type)
	{
		case ast_add: v = "ast_add"; break;
		case ast_sub: v = "ast_sub"; break;
		case ast_mul: v = "ast_mul"; break;
		case ast_div: v = "ast_div"; break;
		case ast_const: v = "ast_const"; break;
		case ast_ident: v = "ast_ident"; break;
		case ast_assign: v = "ast_assign"; break;
		case ast_function: v = "ast_function"; break;
		case ast_return: v = "ast_return"; break;
		case ast_at_asm: v = "ast_at_asm"; break;
		case ast_join: v = "ast_join"; break;
		case ast_noop: v = "ast_noop"; break;
	}
	
	return v;
}

void pretty_ast_tree(ast_T *root, int level)
{
	if (!root) return;

	const char *type = ast_type_as_string(root->type);

	for (int i = 0; i < level; ++i)
		printf("│");

	printf("└");

	if (root->token)
		printf("%s - (%s: %s = %s)",
				type,
				token_type_to_string(root->token->type),
				root->token->value,
				data_type_to_string(root->data_type));
	else printf("AST(%s)", type);

	printf("\n");

	pretty_ast_tree(root->left, level + 1);
	pretty_ast_tree(root->mid, level + 1);
	pretty_ast_tree(root->right, level + 1);
}

parser_T *init_parser(list_T *tokens)
{
	parser_T *parser = malloc(sizeof(parser_T));
	if (!parser)
	{
		printf("err :: init_parser :: failed to allocate memory.\n");
		return NULL;
	}
	parser->tokens = tokens;
	parser->index = 0;
	parser->token = list_get(parser->tokens, parser->index);
	return parser;
}

token_T *parser_token_peek(parser_T *parser, size_t offset)
{
	// peek token from current index point.
	size_t from_offset = parser->index + offset;

	if (from_offset < list_length(parser->tokens))
		return list_get(parser->tokens, from_offset);

	return NULL;
}

token_T *parser_eat(parser_T *parser, token_type_T token_type)
{
	// if current token type matches the token_type,
	// then move to next token, and return next token.
	// if token_type is unknown_token then move to next token.
	
	token_T *token = NULL;
	if (parser->token->type == token_type || token_type == tt_unknown_token)
	{
		token = parser->token;

		parser->index++;
		parser->token  = list_get(parser->tokens, parser->index);
	}
	else
	{
		// TODO: proper error management
		printf("err :: expected `%s`, got `%s`.\n",
			token_type_to_string(token_type), token_type_to_string(parser->token->type)
		);
	}

	return token;
}

int get_token_prec(token_T *token)
{
	switch (token->type)
	{
		case tt_plus:
		case tt_minus: return 1;
		case tt_star:
		case tt_fslash: return 2;
		default: printf("err:: uhh get_token_prec, something?!\n"); return 0;
	}
}

ast_type_T convert_token_type_to_ast_type(token_T *token)
{
	switch (token->type)
	{
		case tt_plus: return ast_add;
		case tt_minus: return ast_sub;
		case tt_star: return ast_mul;
		case tt_fslash: return ast_div;
		default: printf("err :: what operations are you doing? bruh...\n"); return ast_noop;
	}
}

ast_T *parser_parse_primary(parser_T *parser)
{
	switch (parser->token->type)
	{
		case tt_const_int:
			return init_ast_leaf(ast_const, dnil, parser_eat(parser, tt_unknown_token), 0);
		case tt_string:
			return init_ast_leaf(ast_const, dstr, parser_eat(parser, tt_string), 0);
		case tt_ident:
		{
			token_T *ident = parser_eat(parser, tt_ident);
			trie_value_T sv = trie_find(symbol_trie_map, ident->value);
			if (!sv.is_value)
			{
				printf("err :: variable `%s` is not defined.\n", ident->value);
				return NULL;
			}

			return init_ast_leaf(ast_ident, SYMBOLS[sv.value.i32].data_type, ident, sv.value.i32);
		}
		default:
		{
			printf("err :: no primary found.\n");
			parser_eat(parser, tt_unknown_token);
			return NULL;
		}
	}
}

ast_T *parser_parse_expr(parser_T *parser, int tok_prec)
{
	ast_T *left = NULL;

	if (parser->token->type == tt_lparan)
	{
		parser_eat(parser, tt_lparan);
		left = parser_parse_expr(parser, 0);
		parser_eat(parser, tt_rparan);
	}
	else left = parser_parse_primary(parser);

	token_T *token = parser->token;

	if (token->type == tt_semi || token->type == tt_rparan)
		return left;

	while (get_token_prec(token) > tok_prec)
	{
		parser_eat(parser, tt_unknown_token);

		ast_T *right = parser_parse_expr(parser, get_token_prec(token));

		data_type_T new_type =
			type_check(convert_token_type_to_ast_type(token),
			left->data_type, right->data_type);

		left =
			init_ast(convert_token_type_to_ast_type(token),
			new_type, token, left, NULL, right, 0);

		token = parser->token;
		if (token->type == tt_semi || token->type == tt_rparan)
			return left;
	}

	return left;
}

ast_T *parser_parse_assign(parser_T *parser)
{
	token_T *var_name = parser_eat(parser, tt_ident);
	ast_T *ast = init_ast_leaf(ast_assign, dnil, var_name, 0);

	if (parser->token->type == tt_colon)
	{
		parser_eat(parser, tt_colon);

		token_T *var_type = parser_eat(parser, tt_unknown_token);
		data_type_T data_type = token_type_to_data_type(var_type->type);
		if (data_type == dvoid)
		{
			printf("err :: well you cannot put void in variable.\n");
			return NULL;
		}

		trie_value_T sv = trie_find(symbol_trie_map, var_name->value);
		if (sv.is_value)
		{
			printf("err :: variable `%s` already defined.\n", var_name->value);
			return NULL;
		}

		symbol_T symbol = { 0 };
		symbol.symb_s = SVAR;
		symbol.symb_c = CGLOBAL;
		symbol.name = malloc(strlen(var_name->value) + 1);
		symbol.name = strdup(var_name->value);
		symbol.data_type = data_type;

		size_t slot = init_glob_symb(symbol);
		ast->index = slot;
		ast->data_type = data_type;

		trie_insert(symbol_trie_map, var_name->value, (trie_value_T){ .value.i32 = slot });

		if (parser->token->type == tt_semi)
			return ast;
	}

	trie_value_T sv = trie_find(symbol_trie_map, var_name->value);
	if (!sv.is_value)
	{
		printf("err :: variable `%s` not defined.\n", var_name->value);
		return NULL;
	}

	parser_eat(parser, tt_assign);

	ast->left = parser_parse_expr(parser, 0);
	ast->data_type = type_check(ast->type, ast->data_type, ast->left->data_type);

	SYMBOLS[sv.value.i32].data_type = ast->data_type;

	return ast;
}

ast_T *parser_parse_ident(parser_T *parser)
{
	token_T *token = parser_token_peek(parser, 1);

	switch (token->type)
	{
		case tt_colon:
		case tt_assign:
			return parser_parse_assign(parser);
		default:
			printf("err :: well, something is wrong in ident matcher.\n");
			parser_eat(parser, tt_unknown_token);
			return NULL;
	}
}

ast_T *parser_parse_at_statement(parser_T *parser)
{
	parser_eat(parser, tt_at);
	token_T *kind_of_at = parser_eat(parser, tt_ident);
	ast_T *ast = NULL;

	parser_eat(parser, tt_lparan);
	if (!strcmp(kind_of_at->value, "asm"))
		ast = init_ast_leaf(ast_at_asm, dnil, parser_eat(parser, tt_string), 0);
	parser_eat(parser, tt_rparan);

	return ast;
}

ast_T *parser_parse_statement(parser_T *parser)
{
	ast_T *left = NULL;

	switch (parser->token->type)
	{
		case tt_ident: left = parser_parse_ident(parser); break;
		case tt_at: left = parser_parse_at_statement(parser); break;
		default: left = parser_parse_expr(parser, 0);
	}

	return left;
}

ast_T *parser_parse_compound_statement(parser_T *parser)
{
	ast_T *tree;
	ast_T *left = NULL;

	parser_eat(parser, tt_lbrace);

	while (parser->token->type != tt_rbrace)
	{
		switch (parser->token->type)
		{
			case tt_lbrace: tree = parser_parse_compound_statement(parser); break;
			default: tree = parser_parse_statement(parser);
		}

		if (parser->token->type == tt_semi)
			parser_eat(parser, tt_semi);

		if (tree)
		{
			if (!left) left = tree;
			else left = init_ast(ast_join, dnil, NULL, left, NULL, tree, 0);
		}
	}

	parser_eat(parser, tt_rbrace);

	return left;
}

ast_T *parser_parse(parser_T *parser)
{
	// this will contain first statement
	ast_T *tree;
	ast_T *left = NULL;

	while (parser->token->type != tt_eof)
	{
		switch (parser->token->type)
		{
			case tt_lbrace: tree = parser_parse_compound_statement(parser); break;
			default: tree = parser_parse_statement(parser);
		}

		if (parser->token->type == tt_semi)
			parser_eat(parser, tt_semi);

		if (tree)
		{
			if (!left) left = tree;
			else left = init_ast(ast_join, dnil, NULL, left, NULL, tree, 0);
		}
	}

	left =
		// it means the program is empty file.
		!left ? init_ast_unary(ast_noop, dnil, NULL, left, 0) :

		// it means it is not.
		left;

	return left;
}
