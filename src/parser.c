#include "parser.h"

ast_T *init_ast(ast_type_T type, token_T *token, ast_T *left, ast_T *mid, ast_T *right)
{
	ast_T *ast = malloc(sizeof(struct AST_STRUCT));
	if (!ast)
	{
		printf("err :: init_ast :: failed to allocate memory.\n");
		return NULL;
	}
	ast->type = type;
	ast->token = token;
	ast->data_type = dnil;
	ast->left = left;
	ast->mid = mid;
	ast->right = right;
	return ast;
}

ast_T *init_ast_dt(ast_type_T type, token_T *token, data_type_T data_type, ast_T *left, ast_T *mid, ast_T *right)
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
	return ast;
}

ast_T *init_ast_jt(ast_type_T type)
{
	return init_ast(type, NULL, NULL, NULL, NULL);
}

ast_T *init_ast_tt(ast_type_T type, token_T *token)
{
	return init_ast(type, token, NULL, NULL, NULL);
}

ast_T *init_ast_l(ast_type_T type, token_T *token, ast_T *left)
{
	return init_ast(type, token, left, NULL, NULL);
}

ast_T *init_ast_lr(ast_type_T type, token_T *token, ast_T *left, ast_T *right)
{
	return init_ast(type, token, left, NULL, right);
}

ast_T *init_ast_m(ast_type_T type, token_T *token, ast_T *mid)
{
	return init_ast(type, token, NULL, mid, NULL);
}

ast_T *init_ast_tt_dt(ast_type_T type, token_T *token, data_type_T data_type)
{
	return init_ast_dt(type, token, data_type, NULL, NULL, NULL);
}

ast_T *init_ast_l_dt(ast_type_T type, token_T *token, data_type_T data_type, ast_T *left)
{
	return init_ast_dt(type, token, data_type, left, NULL, NULL);
}

ast_T *init_ast_lr_dt(ast_type_T type, token_T *token, data_type_T data_type, ast_T *left, ast_T *right)
{
	return init_ast_dt(type, token, data_type, left, NULL, right);
}

ast_T *init_ast_m_dt(ast_type_T type, token_T *token, data_type_T data_type, ast_T *mid)
{
	return init_ast_dt(type, token, data_type, NULL, mid, NULL);
}

const char *ast_type_as_string(ast_type_T type)
{
	char *v;

	switch (type)
	{
		case ast_program: v = "ast_program"; break;
		case ast_statement: v = "ast_statement"; break;
		case ast_expr: v = "ast_expr"; break;
		case ast_term: v = "ast_term"; break;
		case ast_return: v = "ast_return"; break;
		case ast_var: v = "ast_var"; break;
		case ast_assign: v = "ast_assign"; break;
		case ast_data_type: v = "ast_data_type"; break;
		case ast_class_type: v = "ast_class_type"; break;
		case ast_class_call: v = "ast_class_call"; break;
		case ast_function: v = "ast_function"; break;
		case ast_call: v = "ast_call"; break;
		case ast_ident: v = "ast_ident"; break;
		case ast_const: v = "ast_const"; break;
		case ast_join: v = "ast_join"; break;
		case ast_noop: v = "ast_noop"; break;
	}
	
	return v;
}

const char *data_type_to_string(data_type_T dt)
{
	char *t;

	switch (dt)
	{
		case db: t = "db"; break;
		case dh: t = "dh"; break;
		case dw: t = "dw"; break;
		case dl: t = "dl"; break;
		case ds: t = "ds"; break;
		case dd: t = "dd"; break;
		case dp: t = "dp"; break;
		case dnil: t = "dnil"; break;
	}

	return t;
}

data_type_T tt_dt_to_ast_dt(token_type_T tt_dt)
{
	switch (tt_dt)
	{
		case tt_const_int:
		case tt_i32:
		case tt_u32:
			return dw;
		case tt_i64:
		case tt_u64:
			return dl;
		case tt_const_float:
		case tt_f32:
			return ds;
		case tt_f64:
			return dd;
		case tt_i8:
		case tt_char:
			return db;
		case tt_i16:
		case tt_u16:
			return dh;
		case tt_str:
		case tt_string:
		case tt_ptr:
			return dp;
		default: return dnil;
	}
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
				token_to_string(root->token->type),
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
			token_to_string(token_type), token_to_string(parser->token->type)
		);
	}

	return token;
}

ast_T *parser_parse_call(parser_T *parser)
{
	// f(args, ...)
	
	token_T *id = parser_eat(parser, tt_ident);
	parser_eat(parser, tt_lparan);

	// then we need to consume arguments

	ast_T *left = NULL;
	while (parser->token->type != tt_eof && parser->token->type != tt_rparan)
	{
		if (parser->token->type == tt_comma)
			parser_eat(parser, tt_comma);

		left = init_ast_lr(ast_join, NULL, left, parser_parse_expr(parser));
	}
	parser_eat(parser, tt_rparan);

	return left ? init_ast_l(ast_call, id, left) : init_ast_tt(ast_call, id);
}

ast_T *parser_parse_factor(parser_T *parser)
{
	switch (parser->token->type)
	{
		case tt_lparan:
		{
			parser_eat(parser, tt_lparan);
			ast_T *ast = parser_parse_expr(parser);
			parser_eat(parser, tt_rparan);

			return ast;
		}
		case tt_const_int:
			return init_ast_tt_dt(ast_const, parser_eat(parser, tt_const_int), dw);
		case tt_const_float:
			return init_ast_tt_dt(ast_const, parser_eat(parser, tt_const_float), ds);
		case tt_string:
			return init_ast_tt_dt(ast_const, parser_eat(parser, tt_string), dp);
		case tt_ident:
		{
			// depending on the current token:
			// it might be one of two things,
			// either v::f() or f()
			// if current token is not
			// tt_dcolon or tt_lparan
			// then, ast will be ident.

			if (parser_token_peek(parser, 1)->type == tt_dcolon)
			{
				// class function call

				token_T *id = parser_eat(parser, tt_ident);
				parser_eat(parser, tt_dcolon);

				ast_T *ast = init_ast_l(ast_class_call, id, parser_parse_call(parser));
				
				return ast;
			}
			else if (parser_token_peek(parser, 1)->type == tt_lparan)
				return parser_parse_call(parser);

			token_T *id = parser_eat(parser, tt_ident);
			symb_table_T *vh = hash_get(symb_table, id->value);

			if (!vh)
			{
				printf("err :: variable `%s` not defined.\n", id->value);
				return init_ast_jt(ast_noop);
			}

			return init_ast_tt_dt(ast_ident, id, vh->data_type);
		}
		default: return NULL;
	}
}

ast_T *parser_parse_term(parser_T *parser)
{
	ast_T *left = parser_parse_factor(parser);

	token_T *token;
	while ( (token = parser_token_peek(parser, 0)) && (token->type == tt_star ||
			 token->type == tt_fslash ||
			 token->type == tt_mod))
	{
		token = parser_eat(parser, tt_unknown_token);

		ast_T *right = parser_parse_factor(parser);

		if (!right)
		{
			// TODO: proper error management
			printf("err :: expected `factor` after `*, /, %%`.\n");

			return NULL;
		}

		if (left->data_type != right->data_type)
		{
			printf("err :: `%s` & `%s` are not same type.\n",
					data_type_to_string(left->data_type),
					data_type_to_string(right->data_type)
					);
			return NULL;
		}
		
		if (left->data_type == dp || right->data_type == dp)
		{
			printf("err :: cannot perform math operation on pointer.\n");
			return NULL;
		}

		left = init_ast_lr_dt(ast_term, token, left->data_type, left, right);
	}

	return left;
}

ast_T *parser_parse_expr(parser_T *parser)
{
	ast_T *left = parser_parse_term(parser);

	token_T *token;
	while ((token = parser_token_peek(parser, 0)) && (token->type == tt_plus || token->type == tt_minus))
	{
		token = parser_eat(parser, tt_unknown_token);

		ast_T *right = parser_parse_term(parser);

		if (!right)
		{
			// TODO: proper error management
			printf("err :: expected `term` after `+, -`.\n");

			return NULL;
		}

		if (left->data_type != right->data_type)
		{
			printf("err :: `%s` & `%s` are not same type.\n",
					data_type_to_string(left->data_type),
					data_type_to_string(right->data_type)
					);
			return NULL;
		}
		
		if (left->data_type == dp || right->data_type == dp)
		{
			printf("err :: cannot perform math operation on pointer.\n");
			return NULL;
		}

		left = init_ast_lr_dt(ast_expr, token, left->data_type, left, right);
	}

	return left;
}

ast_T *parser_parse_data_type(parser_T *parser)
{
	// so variable data can be described into two parts:
	// i32, i16, char, str... simple things.
	// then, we have...
	// class_name<types, ...> more complex thing 
	
	ast_T *ast = NULL;

	token_T *vt = parser_eat(parser, tt_unknown_token);
	switch (vt->type)
	{
		case tt_void:
		{
			// because return type will have another function,
			// so void would not go with variable or other things.
			printf("err :: `void` can only be part of return type.\n");

			return NULL;
		}
		case tt_char:
		case tt_str:
		case tt_i8:
		case tt_i16:
		case tt_i32:
		case tt_i64:
		case tt_u8:
		case tt_u16:
		case tt_u32:
		case tt_u64:
		case tt_f32:
		case tt_f64:
		case tt_ptr:
			ast = init_ast_tt_dt(ast_data_type, vt, tt_dt_to_ast_dt(vt->type));
			break;

		default:
		{
			// if it is not generic type
			// and current token is `<` then
			// it must be those complex type.

			if (parser->token->type == tt_lt)
			{
				parser_eat(parser, tt_lt);

				ast = init_ast_l(ast_class_type, vt, parser_parse_data_type(parser));

				while (parser->token->type != tt_gt && parser->token->type != tt_eof)
				{
					parser_eat(parser, tt_comma);
					ast->left = init_ast_lr(ast_join, NULL, ast->left, parser_parse_data_type(parser));
				}

				parser_eat(parser, tt_gt);
			}
		}
	}

	return ast;
}

ast_T *parser_parse_return_type(parser_T *parser)
{
	ast_T *ast = NULL;

	token_T *vt = parser_eat(parser, tt_unknown_token);
	switch (vt->type)
	{
		case tt_void:
		case tt_char:
		case tt_str:
		case tt_i8:
		case tt_i16:
		case tt_i32:
		case tt_i64:
		case tt_u8:
		case tt_u16:
		case tt_u32:
		case tt_u64:
		case tt_f32:
		case tt_f64:
		case tt_ptr:
			ast = init_ast_tt_dt(ast_data_type, vt, tt_dt_to_ast_dt(vt->type));
			break;

		default:
		{
			// if it is not generic type
			// and current token is `<` then
			// it must be those complex type.

			if (parser->token->type == tt_lt)
			{
				parser_eat(parser, tt_lt);

				ast = init_ast_l(ast_class_type, vt, parser_parse_data_type(parser));

				while (parser->token->type != tt_gt && parser->token->type != tt_eof)
				{
					parser_eat(parser, tt_comma);
					ast->left = init_ast_lr(ast_join, NULL, ast->left, parser_parse_data_type(parser));
				}

				parser_eat(parser, tt_gt);
			}
		}
	}

	return ast;
}

ast_T *parser_parse_variable(parser_T *parser)
{
	/*
	 * var_name: type = expr;
	*/
	token_T *var_name = parser_eat(parser, tt_ident);

	parser_eat(parser, tt_colon);

	// data type can be complex.
	ast_T *data_type = parser_parse_data_type(parser);

	symb_table_T *vh = hash_get(symb_table, var_name->value);
	if (vh)
		printf("err :: variable `%s` already defined.\n", var_name->value);
	else
	{
		vh = malloc(sizeof(symb_table_T));
		vh->data_type = data_type->data_type;
		vh->symb_kind = VAR;

		hash_set(symb_table, var_name->value, vh);
	}

	if (parser->token->type != tt_assign)
		return init_ast_l_dt(ast_var, var_name, data_type->data_type, data_type);

	parser_eat(parser, tt_assign);

	ast_T *expr = parser_parse_expr(parser);

	if (!expr)
		return init_ast_l_dt(ast_var, var_name, data_type->data_type, data_type);

	if ((data_type->data_type == dp && expr->data_type != dp) ||
			(data_type->data_type != dp && expr->data_type == dp))
	{
		printf("err :: pointer need pointer on the other side.\n");
		return NULL;
	}

	return init_ast_lr_dt(ast_var, var_name, data_type->data_type, data_type, expr);
}

ast_T *parser_parse_assignment(parser_T *parser)
{
	token_T *var_name = parser_eat(parser, tt_ident);
	parser_eat(parser, tt_assign);

	symb_table_T *vh = hash_get(symb_table, var_name->value);
	if (!vh)
		printf("err :: variable `%s` not defined.\n", var_name->value);

	ast_T *expr = parser_parse_expr(parser);

	return init_ast_l(ast_assign, var_name, expr);
}

ast_T *parser_parse_function(parser_T *parser)
{
	/*
	 * fn_name -> (arg: ...): i32 {
	 * 		compound_stmnt
	 * }
	*/

	token_T *fn_name = parser_eat(parser, tt_ident);

	parser_eat(parser, tt_right_arrow);
	parser_eat(parser, tt_lparan);

	ast_T *args = NULL;

	// parse arguments
	while (parser->token->type != tt_rparan)
	{
		token_T *arg_name = parser_eat(parser, tt_ident);
		parser_eat(parser, tt_colon);
		ast_T *arg_dt = parser_parse_data_type(parser);

		args = init_ast_lr_dt(ast_join, arg_name, arg_dt->data_type, args, arg_dt);

		if (parser->token->type != tt_rparan)
			parser_eat(parser, tt_comma);
	}

	parser_eat(parser, tt_rparan);
	parser_eat(parser, tt_colon);

	// return type are similar to data type,
	// but they have void.
	ast_T *ret_type = parser_parse_return_type(parser);

	ast_T *compound_stmnt = parser_parse_compound_statement(parser);

	return init_ast_dt(ast_function, fn_name, ret_type->data_type, args, ret_type, compound_stmnt);
}

ast_T *parser_parse_return(parser_T *parser)
{
	parser_eat(parser, tt_return);

	ast_T *expr = parser_parse_expr(parser);
	ast_T *ret = init_ast_l_dt(ast_return, NULL, expr->data_type, expr);

	return ret;
}

ast_T *parser_parse_statement(parser_T *parser)
{
	switch (parser->token->type)
	{
		case tt_return:
		{
			ast_T *ast = parser_parse_return(parser);
			parser_eat(parser, tt_semi);
			return ast;
		}
		case tt_ident:
		{
			switch (parser_token_peek(parser, 1)->type)
			{
				case tt_colon:
				{
					ast_T *ast = parser_parse_variable(parser);
					parser_eat(parser, tt_semi);
					return ast;
				}
				case tt_right_arrow: return parser_parse_function(parser);
				case tt_assign:
				{
					ast_T *ast = parser_parse_assignment(parser);
					parser_eat(parser, tt_semi);
					return ast;
				}
				default: ;
			}
		}
		// if none of the cases match,
		// then it must be an expr.
		default:
		{
			ast_T *ast = parser_parse_expr(parser);
			parser_eat(parser, tt_semi);
			return ast;
		}
	}
}

ast_T *parser_parse_compound_statement(parser_T *parser)
{
	ast_T *tree;
	ast_T *left = NULL;

	parser_eat(parser, tt_lbrace);

	while (parser->token->type != tt_rbrace)
	{
		tree = 
			parser->token->type == tt_lbrace ?
			parser_parse_compound_statement(parser) :
			parser_parse_statement(parser);

		if (tree)
		{
			if (!left) left = tree;
			else left = init_ast_lr(ast_join, NULL, left, tree);
		}
	}

	parser_eat(parser, tt_rbrace);

	return left;
}

ast_T *parser_parse_program(parser_T *parser)
{
	// this will contain first statement
	ast_T *tree;
	ast_T *left = NULL;

	while (parser->token->type != tt_eof)
	{
		tree = 
			parser->token->type == tt_lbrace ?
			parser_parse_compound_statement(parser) :
			parser_parse_statement(parser);

		if (tree)
		{
			if (!left) left = tree;
			else left = init_ast_lr(ast_join, NULL, left, tree);
		}
	}

	left =
		// it means the program is empty file.
		!left ? init_ast_l(ast_noop, NULL, left) :

		// it means it is not.
		left;

	return left;
}
