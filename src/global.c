#include "include/global.h"

const char *convert_log_kind_to_string(LOG_KIND kind)
{
	const char *v;

	switch (kind)
	{
		case INFO: v = "INFO"; break;
		case WARN: v = "WANR"; break;
		case ERROR: v = "ERROR"; break;
	}

	return v;
}

void error_flush(LOG_KIND kind, const char *log)
{
	fprintf(stderr, "[%s]: %s\n",
			convert_log_kind_to_string(kind), log
	);
}

const char *__formate_string_function__(char *s, ...)
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

const char *get_term_color(TERM_KIND kind, TERM_COLOR color)
{
	switch (kind)
	{
		case TEXT: return formate_string("\e[0;3%dm", color);
		case BOLD_TEXT: return formate_string("\e[1;3%dm", color);
		case UNDERLINE_TEXT: return formate_string("\e[4;3%dm", color);
		case BACKGROUND: return formate_string("\e[4%dm", color);
		case HIGH_INTEN_BG: return formate_string("\e[0;10%dm", color);
		case HIGH_INTEN_TEXT: return formate_string("\e[0;9%dm", color);
		case BOLD_HIGH_INTEN_TEXT: return formate_string("\e[1;9%dm", color);
		case RESET: return formate_string("\e[0m");
	}
}

symbol_table_T *init_symbol_table(uint16_t scope, symbol_type type, ast_T *node)
{
	symbol_table_T *st = malloc(sizeof(struct SYMBOL_TABLE_STRUCT));
	if (!st)
	{
		perror("cannot allocate memory for symbol: ");
		return NULL;
	}

	st->scope = scope;
	st->type = type;
	st->node = node;

	return st;
}
