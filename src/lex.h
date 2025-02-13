#ifndef __lex_h__
#define __lex_h__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

static const char *reserved_word[] = {
	"import", "impl", "return", "if", "else",
	NULL
};

static const char *data_type[] = {
	"void", "char", "str",
	"i8", "i16", "i32", "i64",
	"u8", "u16", "u32", "u64",
	"f32", "f64",
	"ptr",
	NULL
};

typedef enum {
	// literals
	tt_const_int,
	tt_const_float,
	tt_string,

	// ident
	tt_ident,

	// keywords
	tt_import,
	tt_impl,
	tt_return,
	tt_if,
	tt_else,

	// data_type
	tt_void,
	tt_char,
	tt_str,
	tt_i8,
	tt_i16,
	tt_i32,
	tt_i64,
	tt_u8,
	tt_u16,
	tt_u32,
	tt_u64,
	tt_f32,
	tt_f64,
	tt_ptr,

	// operators
	tt_plus,
	tt_minus,
	tt_star,
	tt_fslash,
	tt_bslash,
	tt_mod,
	tt_semi,
	tt_comma,
	tt_colon,
	tt_dcolon,
	tt_lbrace,
	tt_rbrace,
	tt_lsqb,
	tt_rsqb,
	tt_lparan,
	tt_rparan,
	tt_right_arrow,
	tt_dot,
	tt_assign,
	tt_eq,
	tt_not,
	tt_neq,
	tt_lt,
	tt_lte,
	tt_gt,
	tt_gte,
	tt_dollar,

	// misc tokens
	tt_eof,
	tt_unknown_token
} token_type_T;

typedef struct {
	uint64_t ln;
	uint64_t clm;
	uint8_t len;
} position_T;

typedef struct {
	token_type_T type;
	position_T position;
	char *value;
} token_T;

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
const char *token_to_string(token_type_T type);

// create lexer 
lexer_T *init_lexer(const char *filename);

// get all the tokens in list
list_T *lexer_get_tokens(lexer_T *lexer);

#endif // __lex_h__
