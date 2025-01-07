#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "include/lexer.h"

void lexer_advance(lexer_T *lexer);

token_T *init_token(TokenType type, token_value_T value, uint64_t position)
{
	token_T *ptr = malloc(sizeof(struct TOKEN_STRUCT));
	if (ptr == NULL) return NULL;

	ptr->type = type;
	ptr->value = value;
	ptr->position = position;

	return ptr;
}

lexer_T *init_lexer(const char *source, size_t size)
{
	lexer_T *ptr = malloc(sizeof(struct LEXER_STRUCT));
	if (ptr == NULL) return NULL;

	ptr->source = source;
	ptr->size = size;
	ptr->index = 0;
	ptr->chr = ptr->source[ptr->index];

	return ptr;
}

void lexer_advance(lexer_T *lexer)
{
	lexer->chr = lexer->index < lexer->size ? lexer->source[++lexer->index] : '\0';
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

token_T *current_char_as_token(lexer_T *lexer);

token_T *collect_digit(lexer_T *lexer)
{
	// store lexer position
	size_t st_index = lexer->index - 1;

	size_t bf_size = 0;
	while (isdigit(lexer->chr))
	{
		bf_size++;
		lexer_advance(lexer);
	}

	// restore lexer position
	lexer->index = st_index;
	lexer_advance(lexer); // because index is (-1) so, it will reset chr, too.

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
	token_T *tok = init_token(TT_INT, (token_value_T)(long long int)atoi(bf), st_index);

	// free buffer
	free(bf);

	return tok;
}

token_T *collect_ident(lexer_T *lexer)
{
	// store lexer position
	size_t st_index = lexer->index;

	size_t bf_size = 0;
	while (isalpha(lexer->chr) || lexer->chr == '_')
	{
		bf_size++;
		lexer_advance(lexer);
	}

	// restore lexer position
	lexer->index = st_index;
	lexer_advance(lexer); // because index is (-1) so, it will reset chr, too.
	
	printf("index-> %ld, st_index-> %ld\n", lexer->index, st_index);

	char *bf = malloc(sizeof(char) * (bf_size + 1));

	// capture the ident 
	size_t bf_idx = 0;
	while (isalpha(lexer->chr) || lexer->chr == '_')
	{
		bf[bf_idx++] = lexer->chr;
		lexer_advance(lexer);
	}

	bf[bf_idx] = '\0';

	// create token
	return init_token(TT_ID, (token_value_T)bf, st_index);
}

token_T *current_word_as_token(lexer_T *lexer)
{
	token_T *tok = NULL;

	// check for both cases
	// and call helper functions
	
	if (isdigit(lexer->chr))
		tok = collect_digit(lexer);

	else if (isalpha(lexer->chr) || lexer->chr == '_')
		tok = collect_ident(lexer);

	return tok;
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
			default: fprintf(stderr, "illegal token, `%c` at index `%ld`.\n", lexer->chr, lexer->index); exit(1);
		}
	}

	return init_token(TT_EOF, EOF_TOK, lexer->index);
}
