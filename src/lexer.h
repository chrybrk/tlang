#ifndef __lexer_h__
#define __lexer_h__

#include "glob.h"
#include "list.h"
#include "io.h"

typedef struct
{
	char *filename;
	char *content;
	char current_char;
	uint64_t index;
	uint64_t current_line;
	size_t content_length;
	position_T position;
} lexer_T;

// create token
token_T *init_token(token_type_T type, position_T position, const char *value);

// convert token type to string
const char *token_type_to_string(token_type_T type);

// create lexer 
lexer_T *init_lexer(const char *filename);

// get all the tokens in list
list_T *lexer_get_tokens(lexer_T *lexer);

#endif // __lexer_h__
