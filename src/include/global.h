#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

typedef enum {
	BLACK 	= 0,
	RED 		= 1,
	GREEN 	= 2,
	YELLOW 	= 3,
	BLUE 		= 4,
	MAGENTA = 5,
	CYAN 		= 6,
	WHITE 	= 7
} TERM_COLOR;

typedef enum {
	TEXT = 0,
	BOLD_TEXT,
	UNDERLINE_TEXT,
	BACKGROUND,
	HIGH_INTEN_BG,
	HIGH_INTEN_TEXT,
	BOLD_HIGH_INTEN_TEXT,
	RESET
} TERM_KIND;

#define MAX_ERROR_BATCH 1000
#define formate_string(...) ({ __formate_string_function__(__VA_ARGS__, NULL); })

typedef enum
{
	INFO,
	WARN,
	ERROR
} LOG_KIND;

typedef struct ERROR_STRUCT
{
	LOG_KIND kind;
	const char *log;
} error_T;

extern const char *filename;

void error_flush(LOG_KIND kind, const char *log);

const char *__formate_string_function__(char *s, ...);
const char *get_term_color(TERM_KIND kind, TERM_COLOR color);

#endif // __GLOBAL_H__
