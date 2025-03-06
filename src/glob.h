#ifndef __glob_h__
#define __glob_h__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "trie.h"

// data types for ast node, for type checking
typedef enum {
	dnil,
	dvoid,
	dchar,
	dstr,
	di8,
	di16,
	di32,
	di64,
	du8,
	du16,
	du32,
	du64,
	df32,
	df64
} data_type_T;

// TOKEN TYPES
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
	tt_at,

	// misc tokens
	tt_eof,
	tt_unknown_token
} token_type_T;

// AST TYPES
typedef enum {
	ast_add,
	ast_sub,
	ast_mul,
	ast_div,
	ast_const,
	ast_ident,
	ast_assign,
	ast_function,
	ast_return,
	ast_at_asm,
	ast_join,
	ast_noop
} ast_type_T;

// position in source code
typedef struct {
	uint64_t ln;
	uint64_t clm;
	uint8_t len;
} position_T;

// token
typedef struct {
	token_type_T type;
	position_T position;
	char *value;
} token_T;

typedef enum {
	SVAR,
	SFUNC
} symbol_structure_T;

typedef enum {
	CGLOBAL,
	CLOCAL
} symbol_storage_class_T;

typedef struct SYMBOL_STRUCT {
	symbol_structure_T symb_s;
	symbol_storage_class_T symb_c;
	const char *name;
	data_type_T data_type;
	uint64_t u64;
} symbol_T;

extern trie_node_T *token_trie_map;
extern trie_node_T *symbol_trie_map;
extern uint64_t GLOBAL_INDEX;
extern uint64_t LOCAL_INDEX;
extern uint64_t SYMBOL_SIZE;
extern symbol_T *SYMBOLS;

// string formating
#define formate_string(...) ({ __formate_string_function__(__VA_ARGS__, NULL); })

// macro-related function for formating string
char *__formate_string_function__(char *s, ...);
// string join
char *strjoin(const char *s0, const char *s1);
// string sub-string
char *strsub(const char *s, size_t fp, size_t tp);
// string replace
char *strreplace(char *s, unsigned char from, unsigned char to);
size_t init_glob_symb(symbol_T symbol);
size_t init_locl_symb(symbol_T symbol);
const char *data_type_to_string(data_type_T data_type);
data_type_T token_type_to_data_type(token_type_T token_type);
uint8_t get_data_type_size(data_type_T data_type);
data_type_T type_check(ast_type_T operation, data_type_T left, data_type_T right);

#endif // __glob_h__
