#include "glob.h"

char *__formate_string_function__(char *s, ...)
{
	// allocate small size buffer
	size_t buffer_size = 64; // bytes
	char *buffer = malloc(buffer_size);

	if (buffer == NULL)
	{
		perror("failed to allocate memory for formating string: ");
		return NULL;
	}

	va_list ap;
	va_start(ap, s);

	size_t nSize = vsnprintf(buffer, buffer_size, s, ap);
	if (nSize < 0)
	{
		free(buffer);
		va_end(ap);
	}

	// if buffer does not have enough space then extend it.
	if (nSize >= buffer_size)
	{
		buffer_size = nSize + 1;
		buffer = (char*)realloc(buffer, buffer_size);

		if (buffer == NULL)
		{
			perror("failed to extend buffer for allocating formated string: ");
			va_end(ap);

			return NULL;
		}

		va_start(ap, s);
		vsnprintf(buffer, buffer_size, s, ap);
	}

	va_end(ap);

	return buffer;
}

char *strjoin(const char *s0, const char *s1)
{
	char *s = malloc(strlen(s0) + strlen(s1) + 1);
	s = strcpy(s, s0);
	s = strcat(s, s1);

	return s;
}

char *strsub(const char *s, size_t fp, size_t tp)
{
	if (fp >= tp) return NULL;

	char *sub = malloc((tp - fp) + 1);
	for (size_t i = 0; i < tp - fp; ++i)
		sub[i] = s[fp + i];

	sub[(tp - fp)] = '\0';
	
	return sub;
}

char *strreplace(char *s, unsigned char from, unsigned char to)
{
	for (size_t i = 0; i < strlen(s); ++i)
		if (s[i] == from)
			s[i] = to;

	return s;
}

size_t init_glob_symb(symbol_T symbol)
{
	if (GLOBAL_INDEX >= LOCAL_INDEX)
	{
		printf("err :: cannot store more global symbol.\n");
		return 0;
	}

	SYMBOLS[GLOBAL_INDEX++] = symbol;

	return GLOBAL_INDEX - 1;
}

size_t init_locl_symb(symbol_T symbol)
{
	if (LOCAL_INDEX <= GLOBAL_INDEX)
	{
		printf("err :: cannot store more local symbol.\n");
		return 0;
	}

	SYMBOLS[LOCAL_INDEX--] = symbol;

	return LOCAL_INDEX + 1;
}

const char *data_type_to_string(data_type_T data_type)
{
	char *v;

	switch (data_type)
	{
		case dnil: v = "dnil"; break;
		case dvoid: v = "dvoid"; break;
		case dchar: v = "dchar"; break;
		case dstr: v = "dstr"; break;
		case di8: v = "di8"; break;
		case di16: v = "di16"; break;
		case di32: v = "di32"; break;
		case di64: v = "di64"; break;
		case du8: v = "du8"; break;
		case du16: v = "du16"; break;
		case du32: v = "du32"; break;
		case du64: v = "du64"; break;
		case df32: v = "df32"; break;
		case df64: v = "df64"; break;
	}

	return v;
}

data_type_T token_type_to_data_type(token_type_T token_type)
{
	switch (token_type)
	{
		case tt_void: return dvoid;
		case tt_char: return dchar;
		case tt_str: return dstr;
		case tt_i8: return di8;
		case tt_i16: return di16;
		case tt_i32: return di32;
		case tt_i64: return di64;
		case tt_u8: return du8;
		case tt_u16: return du16;
		case tt_u32: return du32;
		case tt_u64: return du64;
		case tt_f32: return df32;
		case tt_f64: return df64;
		default:
		{
			printf("err :: unrecognised data type found.\n");
			return dnil;
		}
	}
}

uint8_t get_data_type_size(data_type_T data_type)
{
	switch (data_type)
	{
		case dnil:
		case dvoid: 	return 0;

		case dchar:
		case di8:
		case du8: 		return 1;

		case di16:
		case du16: 		return 2;

		case di32:
		case du32:
		case df32:		return 4;

		case di64:
		case du64:
		case df64:
		case dstr: 		return 8;
	}
}

data_type_T type_check(ast_type_T operation, data_type_T left, data_type_T right)
{
	switch (operation)
	{
		case ast_add:
		case ast_sub:
		case ast_mul:
		case ast_div:
		{
			if (left == dstr || right == dstr)
			{
				printf("err :: cannot do (+, -, *, /) on string.\n");
				return dstr;
			}

			return
				get_data_type_size(left) < get_data_type_size(right) ?
				right : left;
		}
		case ast_assign:
		case ast_function:
		{
			if (right == dnil && left != dstr) return left;
			else if (left != right)
			{
				if (
						left == dstr && right != dstr ||
						left != dstr && right == dstr
						)
				{
					printf("err :: %s must have same type as %s type.\n",
							operation == ast_function ? "return" : "expr",
							operation == ast_function ? "function" : "variable"
							);
					return left;
				}
				uint8_t ls = get_data_type_size(left),
								rs = get_data_type_size(right);
				if (ls < rs)
				{
					printf("warn :: lhs has type with lower size than rhs.\n");
					return left;
				}
			}
			return left;
		}
		default:
		{
			return
				get_data_type_size(left) < get_data_type_size(right) ?
				right : left;
		}
	}
}
