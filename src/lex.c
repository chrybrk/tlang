#include "lex.h"

token_type_T check_for_keyword(const char *word)
{
	// check if it is in reserved_word
	
	size_t index = 0;
	while (reserved_word[index])
	{
		if (!strcmp(word, reserved_word[index]))
			break;

		index++;
	}

	if (reserved_word[index] == NULL)
	{
		// then check in data type
		
		index = 0;
		while (data_type[index])
		{
			if (!strcmp(word, data_type[index]))
				break;

			index++;
		}

		if (data_type[index] == NULL)
			return tt_unknown_token;

		return tt_void + index;
	}

	return tt_import + index;
}

// create token
token_T *init_token(token_type_T type, position_T position, const char *value)
{
	token_T *token = malloc(sizeof(token_T));
	token->type = type;
	token->position = position;
	token->value = malloc(strlen(value) + 1);
	token->value = strdup(value);
	return token;
}

const char *token_to_string(token_type_T type)
{
	const char *token2string;

	switch (type)
	{
		case tt_const_int: token2string = "tt_const_int"; break;
		case tt_const_float: token2string = "tt_const_float"; break;
		case tt_string: token2string = "tt_string"; break;
		case tt_ident: token2string = "tt_ident"; break;
		case tt_import: token2string = "tt_import"; break;
		case tt_impl: token2string = "tt_impl"; break;
		case tt_return: token2string = "tt_return"; break;
		case tt_if: token2string = "tt_if"; break;
		case tt_else: token2string = "tt_else"; break;
		case tt_plus: token2string = "tt_plus"; break;
		case tt_void: token2string = "tt_void"; break;
		case tt_char: token2string = "tt_char"; break;
		case tt_str: token2string = "tt_str"; break;
		case tt_i8: token2string = "tt_i8"; break;
		case tt_i16: token2string = "tt_i16"; break;
		case tt_i32: token2string = "tt_i32"; break;
		case tt_i64: token2string = "tt_i64"; break;
		case tt_u8: token2string = "tt_u8"; break;
		case tt_u16: token2string = "tt_u16"; break;
		case tt_u32: token2string = "tt_u32"; break;
		case tt_u64: token2string = "tt_u64"; break;
		case tt_f32: token2string = "tt_f32"; break;
		case tt_f64: token2string = "tt_f64"; break;
		case tt_ptr: token2string = "tt_ptr"; break;
		case tt_minus: token2string = "tt_minus"; break;
		case tt_star: token2string = "tt_star"; break;
		case tt_fslash: token2string = "tt_fslash"; break;
		case tt_bslash: token2string = "tt_bslash"; break;
		case tt_mod: token2string = "tt_mod"; break;
		case tt_semi: token2string = "tt_semi"; break;
		case tt_comma: token2string = "tt_comma"; break;
		case tt_colon: token2string = "tt_colon"; break;
		case tt_dcolon: token2string = "tt_dcolon"; break;
		case tt_lbrace: token2string = "tt_lbrace"; break;
		case tt_rbrace: token2string = "tt_rbrace"; break;
		case tt_lsqb: token2string = "tt_lsqb"; break;
		case tt_rsqb: token2string = "tt_rsqb"; break;
		case tt_lparan: token2string = "tt_lparan"; break;
		case tt_rparan: token2string = "tt_rparan"; break;
		case tt_right_arrow: token2string = "tt_right_arrow"; break;
		case tt_dot: token2string = "tt_dot"; break;
		case tt_assign: token2string = "tt_assign"; break;
		case tt_eq: token2string = "tt_eq"; break;
		case tt_not: token2string = "tt_not"; break;
		case tt_neq: token2string = "tt_neq"; break;
		case tt_lt: token2string = "tt_lt"; break;
		case tt_lte: token2string = "tt_lte"; break;
		case tt_gt: token2string = "tt_gt"; break;
		case tt_gte: token2string = "tt_gte"; break;
		case tt_dollar: token2string = "tt_dollar"; break;
		case tt_eof: token2string = "tt_eof"; break;
		case tt_unknown_token: token2string = "tt_unknown_token"; break;
	}

	return token2string;
}

// create lexer 
lexer_T *init_lexer(const char *filename)
{
	lexer_T *lexer = malloc(sizeof(lexer_T));
	lexer->filename = malloc(strlen(filename) + 1);
	lexer->filename = strdup(filename);
	lexer->content = read_file(filename);
	lexer->content_length = strlen(lexer->content) + 1;
	lexer->index = 0;
	lexer->current_char = lexer->content[lexer->index];
	lexer->position = (position_T){ .ln = 1, .clm = 1, .len = 0 };
	return lexer;
}

void lexer_advance(lexer_T *lexer)
{
	// update position
	if (lexer->current_char == '\n')
		lexer->position.ln++, lexer->position.clm = 1, lexer->current_line = lexer->index + 1;
	else
		lexer->position.clm++;

	// update current character
	lexer->current_char = lexer->content[++lexer->index];
}

void lexer_skip_whitespaces(lexer_T *lexer)
{
	while (
		lexer->current_char == ' ' 		||
		lexer->current_char == '\n' 	||
		lexer->current_char == '\t' 	||
		lexer->current_char == 10 		||
		lexer->current_char == 32
	) lexer_advance(lexer);
}

token_T *lexer_lex_digit(lexer_T *lexer)
{
	// digit can refer to int or floats
	
	position_T position = lexer->position;
	
	lexer_T *tmp = malloc(sizeof(lexer_T));
	memcpy(tmp, lexer, sizeof(lexer_T));

	size_t word_size = 0;
	uint16_t dot_count = 0;

	while (isdigit(tmp->current_char) || tmp->current_char == '.')
	{
		if (tmp->current_char == '.') dot_count++;
		word_size++;

		lexer_advance(tmp);
	}

	// TODO: no longer need for `tmp`

	if (dot_count > 1)
	{
		// TODO: proper error management,
		// for now just print the damn thing.
		printf("err :: `.` more than one dot found in the number lit.\n");
		return NULL;
	}

	char *buffer = malloc(word_size + 1);
	size_t idx = 0;
	while (isdigit(lexer->current_char) || lexer->current_char == '.')
	{
		buffer[idx++] = lexer->current_char;
		lexer_advance(lexer);
	}

	buffer[idx] = '\0';

	token_T *token = init_token(
			dot_count ? tt_const_float : tt_const_int,
			position,
			buffer
	);

	return token;
}

token_T *lexer_lex_ident_or_keyword(lexer_T *lexer)
{
	position_T position = lexer->position;
	
	lexer_T *tmp = malloc(sizeof(lexer_T));
	memcpy(tmp, lexer, sizeof(lexer_T));

	size_t word_size = 0;

	while (isalnum(tmp->current_char) || tmp->current_char == '_')
	{
		word_size++;
		lexer_advance(tmp);
	}

	// TODO: no longer need for `tmp`
	
	char *buffer = malloc(word_size + 1);
	size_t idx = 0;
	while (isalnum(lexer->current_char) || lexer->current_char == '_')
	{
		buffer[idx++] = lexer->current_char;
		lexer_advance(lexer);
	}

	buffer[idx] = '\0';

	token_type_T tt = check_for_keyword(buffer);
	token_T *token = init_token(
			tt == tt_unknown_token ? tt_ident : tt,
			position,
			buffer
	);

	return token;
}

token_T *lexer_lex_word(lexer_T *lexer)
{
	// it could be `ident`, `lit` or `keyword`
	if (isdigit(lexer->current_char))
		return lexer_lex_digit(lexer);
	else if (isalnum(lexer->current_char) || lexer->current_char == '_')
		return lexer_lex_ident_or_keyword(lexer);

	return NULL;
}

token_T *lexer_lex_char(lexer_T *lexer, token_type_T tt)
{
	char *buffer = malloc(2);
	buffer[0] = lexer->current_char;
	buffer[1] = '\0';

	token_T *token = init_token(tt, lexer->position, buffer);
	lexer_advance(lexer);

	return token;
}

token_T *lexer_lex_tchar(lexer_T *lexer, unsigned char nc, token_type_T tt_a, token_type_T tt_b)
{
	position_T position = lexer->position;
	unsigned char oc = lexer->current_char;

	lexer_advance(lexer);

	char *buffer = NULL;
	token_T *token = NULL;

	if (lexer->current_char == nc)
	{
		buffer = malloc(3);

		buffer[0] = oc;
		buffer[1] = nc;
		buffer[2] = '\0';

		lexer_advance(lexer);

		token = init_token(tt_b, position, buffer);
	}
	else
	{
		buffer = malloc(2);
		buffer[0] = oc;
		buffer[1] = '\0';

		token = init_token(tt_a, position, buffer);
	}

	return token;
}

token_T *lexer_lex_string(lexer_T *lexer)
{
	// TODO: implement espace seq thingy
	
	position_T position = lexer->position;

	lexer_advance(lexer);
	
	lexer_T *tmp = malloc(sizeof(lexer_T));
	memcpy(tmp, lexer, sizeof(lexer_T));

	size_t word_size = 0;
	while (tmp->current_char != '"')
	{
		word_size++;
		lexer_advance(tmp);
	}

	// TODO: no need for tmp

	char *buffer = malloc(word_size + 1);
	size_t idx = 0;

	while (lexer->current_char != '"')
	{
		buffer[idx++] = lexer->current_char;
		lexer_advance(lexer);
	}

	buffer[idx] = '\0';

	lexer_advance(lexer);

	token_T *token = init_token(
			tt_string,
			position,
			buffer
	);

	return token;
}

// get all the tokens in list
list_T *lexer_get_tokens(lexer_T *lexer)
{
	// create list of tokens
	list_T *tokens = init_list(sizeof(token_T));

	while (lexer->current_char)
	{
		// check for any whitespaces
		lexer_skip_whitespaces(lexer);

		// check if it needs to be lexed as whole, if not
		if (
				isdigit(lexer->current_char) ||
				isalpha(lexer->current_char) ||
				lexer->current_char == '_'
		) list_push(tokens, lexer_lex_word(lexer));

		// then it must be single character token
		else
		{
			switch (lexer->current_char)
			{
				case '+': list_push(tokens, lexer_lex_char(lexer, tt_plus)); break;
				case '*': list_push(tokens, lexer_lex_char(lexer, tt_star)); break;
				case '/': list_push(tokens, lexer_lex_char(lexer, tt_fslash)); break;
				case '\\': list_push(tokens, lexer_lex_char(lexer, tt_bslash)); break;
				case '%': list_push(tokens, lexer_lex_char(lexer, tt_mod)); break;
				case ';': list_push(tokens, lexer_lex_char(lexer, tt_semi)); break;
				case ',': list_push(tokens, lexer_lex_char(lexer, tt_comma)); break;
				case '{': list_push(tokens, lexer_lex_char(lexer, tt_lbrace)); break;
				case '}': list_push(tokens, lexer_lex_char(lexer, tt_rbrace)); break;
				case '[': list_push(tokens, lexer_lex_char(lexer, tt_lsqb)); break;
				case ']': list_push(tokens, lexer_lex_char(lexer, tt_rsqb)); break;
				case '(': list_push(tokens, lexer_lex_char(lexer, tt_lparan)); break;
				case ')': list_push(tokens, lexer_lex_char(lexer, tt_rparan)); break;
				case '.': list_push(tokens, lexer_lex_char(lexer, tt_dot)); break;
				case '$': list_push(tokens, lexer_lex_char(lexer, tt_dollar)); break;
				case '-': list_push(tokens, lexer_lex_tchar(lexer, '>', tt_minus, tt_right_arrow)); break;
				case ':': list_push(tokens, lexer_lex_tchar(lexer, ':', tt_colon, tt_dcolon)); break;
				case '=': list_push(tokens, lexer_lex_tchar(lexer, '=', tt_assign, tt_eq)); break;
				case '<': list_push(tokens, lexer_lex_tchar(lexer, '=', tt_lt, tt_lte)); break;
				case '>': list_push(tokens, lexer_lex_tchar(lexer, '=', tt_gt, tt_gte)); break;
				case '!': list_push(tokens, lexer_lex_tchar(lexer, '=', tt_not, tt_neq)); break;
				case '"': list_push(tokens, lexer_lex_string(lexer)); break;
				default:
				{
					token_T *token = lexer_lex_char(lexer, tt_unknown_token);
					list_push(tokens, token);

					// TODO: proper error management
					printf(
							"err :: unknown token `%s` (%ld:%ld).\n",
							token->value, token->position.ln, token->position.clm
					);
				}
			}
		}
	}

	list_push(tokens, init_token(tt_eof, lexer->position, ""));

	return tokens;
}
