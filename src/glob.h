#ifndef __glob_h__
#define __glob_h__

#include "utils.h"

extern hash_T *symb_table;

typedef enum {
	db,		// byte 					or char
	dh,		// half-word 			or i16
	dw,		// word 					or i32
	dl,		// long 					or i64
	ds,		// single 				or f32
	dd,		// double 				or f64
	dp,		// pointer 				or ptr 		or str
	dnil	// no-data-type 	or void
} data_type_T;

typedef enum {
	VAR,
	FUNC
} symb_kind_T;

typedef struct {
	data_type_T data_type;
	symb_kind_T symb_kind;
} symb_table_T;

#endif // __glob_h__
