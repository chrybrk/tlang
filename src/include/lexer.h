#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum
{
	TT_ID, 							// identifier
	TT_INT,							// integer
	TT_STRING,					// string 
	TT_STAR,						// star `*`
	TT_MOD,							// mod `%`
	TT_POWER,						// power `^`
	TT_LXOR,						// lg xor `~`
	TT_LAND,						// lg and `&`
	TT_LOR,							// lg or `|`
	TT_LNOT,						// lnot or `!`
	TT_FSLASH,					// forward slash `/`
	TT_BSLASH,					// backward slash `\`
	TT_PLUS,						// plus `+`
	TT_MINUS,						// minus `-`
	TT_COLON,						// colon `:`
	TT_COMMA,						// comma `,`
	TT_SEMI,						// semicolon `;`
	TT_LP,							// left-paran `(`
	TT_RP,							// right-paran `)`
	TT_LS,							// left-square `[`
	TT_RS,							// right-square `]`
	TT_LB,							// left-brace `{`
	TT_RB,							// right-brace `}`
	TT_EQ,							// equal `==`
	TT_NE,							// not-equal `!=`
	TT_LT,							// less than `<`
	TT_GT,							// greater than `>`
	TT_LTE,							// less than equal `<=`
	TT_GTE,							// greater than `>=`
	TT_AND,							// and `&&`
	TT_OR,							// or `||`
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
	uint64_t line_cur;
	position_T position;
} lexer_T;

// create token
token_T *init_token(token_type_T type, const char *value, position_T position);

// create lexer
lexer_T *init_lexer(const char *source);

// generate token
token_T *next_token(lexer_T *lexer);

// get current working line
const char *get_current_line(lexer_T *lexer);

#endif // __LEXER_H__
