#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum
{
	TT_ID, 							// identifier
	TT_INT,							// integer
	TT_STAR,						// star `*`
	TT_FSLASH,					// forward slash `/`
	TT_BSLASH,					// backward slash `\`
	TT_PLUS,						// plus `+`
	TT_MINUS,						// minus `-`
	TT_COLON,						// colon `:`
	TT_SEMI,						// semicolon `;`
	TT_LP,							// left-paran `(`
	TT_RP,							// right-paran `)`
	TT_EQ,							// equal `==`
	TT_ASSIGN,					// assignment `=`
	TT_EOF							// end-of-file
} token_type_T;

typedef struct POSITION_STRUCT
{
	uint64_t line;
	uint64_t column;
	uint8_t length;
} position_T;

typedef struct TOKEN_STRUCT
{
	token_type_T type;
	const char *value;
	position_T position;
} token_T;

typedef struct LEXER_STRUCT
{
	const char *source;
	size_t size;
	char chr;
	uint64_t index;
	position_T position;
} lexer_T;

// create token
token_T *init_token(token_type_T type, const char *value, position_T position);

// create lexer
lexer_T *init_lexer(const char *source);

// generate token
token_T *next_token(lexer_T *lexer);

#endif // __LEXER_H__
