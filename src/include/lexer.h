#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum
{
	TT_ID, 							// identifier
	TT_INT,							// integer
	TT_COLON,						// colon
	TT_SEMI,						// semicolon
	TT_LP,							// left-paran
	TT_RP,							// right-paran
	TT_EQ,							// equal
	TT_EOF							// end-of-file
} TokenType;

typedef union
{
	char *vs;
	char vc;
	long long int vi;
	double vf;
} token_value_T;

#define EOF_TOK (token_value_T)((char)'\0')

typedef struct TOKEN_STRUCT
{
	TokenType type;
	token_value_T value;
	struct
	{
		uint64_t line;
		uint64_t column;
		uint8_t length;
	} position;
} token_T;

typedef struct LEXER_STRUCT
{
	const char *source;
	size_t size;
	uint64_t index;
	struct
	{
		uint64_t line;
		uint64_t column;
	} position;
	char chr;
} lexer_T;

token_T *init_token(TokenType type, token_value_T value, uint64_t line, uint64_t column, uint8_t length);
lexer_T *init_lexer(const char *source, size_t size);
token_T *next_token(lexer_T *lexer);
void token_show(token_T *token);

#endif // __LEXER_H__
