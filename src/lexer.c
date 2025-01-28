#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "include/helper.h"
#include "include/global.h"
#include "include/lexer.h"

token_T *init_token(token_type_T type, const char *value, position_T position)
{
	token_T *ptr = malloc(sizeof(struct TOKEN_STRUCT));
	if (ptr == NULL) return NULL;

	ptr->type = type;
	ptr->value = value;
	ptr->position = position;

	if (ptr->value)
	{
		ptr->position.column = ptr->position.column - strlen(ptr->value);
		ptr->position.length = strlen(ptr->value);
	}

	return ptr;
}

lexer_T *init_lexer(const char *source)
{
	lexer_T *ptr = malloc(sizeof(struct LEXER_STRUCT));
	if (ptr == NULL) return NULL;

	ptr->source = source;
	ptr->size = strlen(ptr->source) + 1;
	ptr->index = 0;
	ptr->line_cur = 0;
	ptr->chr = ptr->source[ptr->index];
	ptr->position.line = 1;
	ptr->position.column = 1;

	return ptr;
}

const char *get_current_line(lexer_T *lexer)
{
	uint64_t from = lexer->line_cur;
	uint64_t to = from;

	while (lexer->source[to] && lexer->source[to] != '\n')
		to++;

	uint64_t size = to - from;
	char *line = malloc(size + 1);

	for (uint64_t i = from; i < to; ++i)
		line[i - from] = lexer->source[i];

	line[size] = '\0';
	
	return line;
}

void lexer_advance(lexer_T *lexer)
{
	// update position
	if (lexer->chr == '\n')
		lexer->position.line++, lexer->position.column = 1, lexer->line_cur = lexer->index + 1;
	else
		lexer->position.column++;

	// update current character
	lexer->chr = lexer->source[++lexer->index];
}

bool char_in_lst(const char ch, size_t size, const char ch_lst[size])
{
	for (size_t i = 0; i < size; ++i)
		if (ch_lst[i] == ch) return true;

	return false;
}

void lexer_skip_whitespaces(lexer_T *lexer)
{
	const char whitespaces[] = {
		' ',
		'\n',
		'\t',
		10,
		32
	};

	while (char_in_lst(lexer->chr, sizeof(whitespaces), whitespaces))
		lexer_advance(lexer);
}

token_T *current_char_as_token(lexer_T *lexer, token_type_T type)
{
	char *s = malloc(2);
	s[0] = lexer->chr;
	s[1] = '\0';

	lexer_advance(lexer);

	token_T *tok = init_token(
			type,
			s,
			lexer->position
	);

	return tok;
}

size_t calc_buffer_size(lexer_T *lexer, int (*cond)(int))
{
	// copy lexer to another memory
	lexer_T *lexer_cp = init_lexer(lexer->source);
	memcpy(lexer_cp, lexer, sizeof(struct LEXER_STRUCT));

	size_t bf_size = 0;
	while (cond(lexer_cp->chr))
	{
		bf_size++;
		lexer_advance(lexer_cp);
	}

	// TODO: free lexer
	
	return bf_size;
}

token_T *collect_digit(lexer_T *lexer)
{
	size_t bf_size = calc_buffer_size(lexer, isdigit);
	char *bf = malloc(sizeof(char) * (bf_size + 1));

	// capture the digits
	size_t bf_idx = 0;
	while (isdigit(lexer->chr))
	{
		bf[bf_idx++] = lexer->chr;
		lexer_advance(lexer);
	}

	bf[bf_idx] = '\0';

	// convert string to number, and create token
	token_T *tok = init_token(
			TT_INT,
			strdup(bf),
			lexer->position
	);

	// free buffer
	free(bf);

	return tok;
}

token_T *collect_ident(lexer_T *lexer)
{
	size_t bf_size = calc_buffer_size(lexer, isident);
	char *bf = malloc(sizeof(char) * (bf_size + 1));

	// capture the ident 
	size_t bf_idx = 0;
	while (isalnum(lexer->chr) || lexer->chr == '_')
	{
		bf[bf_idx++] = lexer->chr;
		lexer_advance(lexer);
	}

	bf[bf_idx] = '\0';

	// create token
	return init_token(
			TT_ID,
			bf,
			lexer->position
	);
}

token_T *current_word_as_token(lexer_T *lexer)
{
	token_T *tok = NULL;

	// check for both cases
	// and call helper functions
	
	if (isdigit(lexer->chr))
		tok = collect_digit(lexer);

	else if (isalnum(lexer->chr) || lexer->chr == '_')
		tok = collect_ident(lexer);

	return tok;
}

token_T *current_string_as_token(lexer_T *lexer)
{
	lexer_advance(lexer);

	size_t bf_size = calc_buffer_size(lexer, isnotdq);
	char *bf = malloc(sizeof(char) * (bf_size + 1));

	// capture the ident 
	size_t bf_idx = 0;
	while (isnotdq(lexer->chr))
	{
		bf[bf_idx++] = lexer->chr;
		lexer_advance(lexer);
	}

	bf[bf_idx] = '\0';

	// create token
	return init_token(
			TT_STRING,
			bf,
			lexer->position
	);
}

token_T
*current_two_char_as_token(
		lexer_T *lexer,
		char nc,
		token_type_T if_tt,
		token_type_T else_tt
)
{
	// current char
	char cc = lexer->chr;

	lexer_advance(lexer);

	if (lexer->chr == nc)
	{
		char *s = malloc(3);
		s[0] = cc;
		s[1] = lexer->chr;
		s[2] = '\0';

		lexer_advance(lexer);

		return init_token(else_tt, s, lexer->position);
	}

	char *s = malloc(2);
	s[0] = cc;
	s[1] = '\0';

	return init_token(if_tt, s, lexer->position);
}

token_T *next_token(lexer_T *lexer)
{
	lexer_skip_whitespaces(lexer);

	while (lexer->chr)
	{
		if (isdigit(lexer->chr) || isalpha(lexer->chr) || lexer->chr == '_')
			return current_word_as_token(lexer);

		switch (lexer->chr)
		{
			case '*': return current_char_as_token(lexer, TT_STAR);
			case '%': return current_char_as_token(lexer, TT_MOD);
			case '^': return current_char_as_token(lexer, TT_POWER);
			case '~': return current_char_as_token(lexer, TT_LXOR);
			case '/': return current_char_as_token(lexer, TT_FSLASH);
			case '\\': return current_char_as_token(lexer, TT_BSLASH);
			case '+': return current_char_as_token(lexer, TT_PLUS);
			case '-': return current_char_as_token(lexer, TT_MINUS);
			case ':': return current_char_as_token(lexer, TT_COLON);
			case ',': return current_char_as_token(lexer, TT_COMMA);
			case ';': return current_char_as_token(lexer, TT_SEMI);
			case '(': return current_char_as_token(lexer, TT_LP);
			case ')': return current_char_as_token(lexer, TT_RP);
			case '[': return current_char_as_token(lexer, TT_LS);
			case ']': return current_char_as_token(lexer, TT_RS);
			case '{': return current_char_as_token(lexer, TT_LB);
			case '}': return current_char_as_token(lexer, TT_RB);
			case '&': return current_two_char_as_token(lexer, '&', TT_LAND, TT_AND);
			case '|': return current_two_char_as_token(lexer, '|', TT_LOR, TT_OR);
			case '=': return current_two_char_as_token(lexer, '=', TT_ASSIGN, TT_EQ);
			case '!': return current_two_char_as_token(lexer, '=', TT_LNOT, TT_NE);
			case '<': return current_two_char_as_token(lexer, '=', TT_LT, TT_LTE);
			case '>': return current_two_char_as_token(lexer, '=', TT_GT, TT_GTE);
			case '"': return current_string_as_token(lexer);
			default:
			{
				const char *log =
					formate_string(
							"%s:%ld:%ld: %serror%s: unknown character `%c`\n\t %ld | %s\n",
							filename,
							lexer->position.line, lexer->position.column,
							get_term_color(BOLD_TEXT, RED), get_term_color(RESET, WHITE),
							lexer->chr,
							lexer->position.line, get_current_line(lexer)
					);
				error_flush(ERROR, log);
				lexer_advance(lexer);
				lexer_skip_whitespaces(lexer);
			}
		}
	}

	return init_token(TT_EOF, NULL, lexer->position);
}
