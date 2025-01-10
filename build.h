/**********************************************************************************************
* build.h (1.0.0) - A simple yet powerful build system written in C.
*
* FEATURES:
* 	- No external dependencies.
* 	- Automatic memory management using arena allocator.
* 	- Automatic updates binary of build system: once compiled upon running it will check for any updates in the build-system.
* 	- Arena allocator and Dynamic array.
* 	- String function: join, seperate, sub-string.
* 	- Get list of files, Create directories, check if files has been modified.
* 	- Execute commands with strings, fromated strings.
*
* NOTES:
*
* 	#define IMPLEMENT_BUILD_H
* 	#include "build.h"
* 	
*		const char *build_source;
*		const char *build_bin;
*
*		These two must be set in the source file.
*
*		void build_start(int argc, char **argv)
*		{
*		}
*
*		This is the function that will start executing.
*
* MIT License
*
* Copyright (c) 2024 Chry003
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
**********************************************************************************************/

#define IMPLEMENT_BUILD_H
#ifdef IMPLEMENT_BUILD_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <dirent.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/wait.h>

// TODO: make like dependencies system

// called at the end of scope
#define defer(func) __attribute__((cleanup(func)))

// string formating
#define formate_string(...) ({ __formate_string_function__(__VA_ARGS__, NULL); })

// log-system
#define INFO(...) printf("%s[BUILD :: INFO]:%s %s\n", get_term_color(TEXT, GREEN), get_term_color(RESET, 0), string_cstr(formate_string(__VA_ARGS__)))
#define WARN(...) printf("%s[BUILD :: WARN]:%s %s\n", get_term_color(TEXT, YELLOW), get_term_color(RESET, 0), string_cstr(formate_string(__VA_ARGS__)))
#define ERROR(...) printf("%s[BUILD :: ERROR]:%s %s\n", get_term_color(TEXT, RED), get_term_color(RESET, 0), string_cstr(formate_string(__VA_ARGS__))), exit_build_system(1);

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

// arena allocator
typedef struct ARENA_STRUCT
{
	size_t buffer_size;		// size of buffer
	void *buffer;					// buffer
	size_t ptr;						// current pointer in buffer
} arena_T;

// array
typedef struct ARRAY_STRUCT
{
	void **buffer;				// buffer to store elements
	size_t index;					// index (pointer) where we need to store (actual length of array)
	size_t len;						// length of the buffer (not true length)
	size_t item_size;			// size of the item that needs to be put
} array_T;

// string
typedef struct STRING_STRUCT
{
	char *string;					// string data
	size_t len;						// length of string
} string_T;

/**********************************************************************************************
*																   Things that needs to be global
**********************************************************************************************/

// function that needs to implemented by the source
extern void build_start(int argc, char **argv);

// build files
extern const char *build_source;
extern const char *build_bin;

// warn for non-freed arrays 
static int array_allocated_counter = 0;

// color for logging
const char *get_term_color(TERM_KIND kind, TERM_COLOR color);

// macro-related function for formating string
string_T *__formate_string_function__(char *s, ...);

// arena that will be used for allocating most of the things.
static arena_T *build_main_arena;

// create arena allocator
arena_T *init_arena(size_t n);

// allocate buffer on arena
void *arena_allocate(arena_T *arena, size_t n);

// free the allocator
void arena_free(arena_T **arena);

// create new array
array_T *init_array(size_t item_size);

// push new element in array
void array_push(array_T *array, void *item);

// pop last element from the array
void *array_pop(array_T *array);

// get the element from the array
void *array_get(array_T *array, size_t index);

// take n element from array
array_T *array_take(array_T *array, unsigned int n);

// drop n element from array
array_T *array_drop(array_T *array, unsigned int n);

// free array
void array_free(array_T *array);

// create new string
string_T *init_string(const char *s);

// join two string, return new string 
string_T *string_join(string_T *s0, string_T *s1);

// create new sub-string
string_T *string_substr(string_T *s, size_t from_pos, size_t to_pos);

// change whitespace to seperator, return s
string_T *string_add_seprator(string_T *s, unsigned char seperator, unsigned char replace);

// seperates string into list 
array_T *string_seperate(string_T *s, unsigned char seperator);

// create new string from string list
string_T *string_from_string_list(const char **s, size_t len);

// create string from array
string_T *string_from_array(array_T *array);

// create char * from string
const char *string_cstr(string_T *s);

// get char at index
char string_at(string_T *s, size_t i);

// compare two string
bool string_compare(string_T *s0, string_T *s1);

// compare one string within the list
bool string_compare_with_list(string_T *s, array_T *list);

// exectue command in the shell
bool command_execute(string_T *command);

// get list of files
array_T *file_get(string_T *path);

// get list of files (ends with)
array_T *file_get_end(string_T *path, string_T *end);

// check if file exist
bool file_exist(string_T *path);

// check if directory exist
bool directory_exist(string_T *path);

// create new directory (it will create all directory in the path)
void create_directory(string_T *path);

// get last modified time of file
time_t get_time_from_file(string_T *path);

// checks if binary source has changed
bool binary_test(string_T *binary_path, array_T *source_list);

/**********************************************************************************************
*																   The Actual Implementation
**********************************************************************************************/

const char *get_term_color(TERM_KIND kind, TERM_COLOR color)
{
	switch (kind)
	{
		case TEXT: return string_cstr(formate_string("\e[0;3%dm", color));
		case BOLD_TEXT: return string_cstr(formate_string("\e[1;3%dm", color));
		case UNDERLINE_TEXT: return string_cstr(formate_string("\e[4;3%dm", color));
		case BACKGROUND: return string_cstr(formate_string("\e[4%dm", color));
		case HIGH_INTEN_BG: return string_cstr(formate_string("\e[0;10%dm", color));
		case HIGH_INTEN_TEXT: return string_cstr(formate_string("\e[0;9%dm", color));
		case BOLD_HIGH_INTEN_TEXT: return string_cstr(formate_string("\e[1;9%dm", color));
		case RESET: return string_cstr(formate_string("\e[0m"));
	}
}

string_T *__formate_string_function__(char *s, ...)
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

	string_T *string = init_string(buffer);
	free(buffer);

	return string;
}

arena_T *init_arena(size_t n)
{
	if (n <= 0) return NULL;

	arena_T *new = malloc(sizeof(struct ARENA_STRUCT));
	if (new == NULL)
	{
		perror("failed to allocate memory for arena: ");
		return NULL;
	}

	new->buffer_size = n;
	new->buffer = malloc(new->buffer_size);
	new->ptr = 0;

	if (new->buffer == NULL)
	{
		perror("failed to allocate memory for arena buffer: ");

		fprintf(stderr, "trying to allocate less memory for arena buffer.\n");

		while (new->buffer_size > (1 * 1024))
		{
			new->buffer_size /= 2;
			fprintf(stderr, "allocating %zu for arena buffer.\n", new->buffer_size);

			new->buffer = malloc(new->buffer_size);
			if (new->buffer) return new;
		}

		fprintf(stderr, "failed to allocate less-memory for arena.\n");
		return NULL;
	}

	return new;
}

void *arena_allocate(arena_T *arena, size_t n)
{
	if (arena->ptr + n < arena->buffer_size)
	{
		void *addr = arena->buffer + arena->ptr;
		arena->ptr += n;

		return addr;
	}

	fprintf(stderr, "max memory reached for arena.\n");
	return NULL;
}

void arena_free(arena_T **arena)
{
	if (*arena)
		free(((arena_T*)*arena)->buffer);
}

array_T *init_array(size_t item_size)
{
	array_T *array = malloc(sizeof(struct ARRAY_STRUCT));
	if (array == NULL)
	{
		perror("failed to allocate memory for array: ");
		return NULL;
	}

	// initially allocate enough for 10 elements.
	array->len = 10;
	array->item_size = item_size;
	array->buffer = malloc(array->item_size * array->len);
	if (array->buffer == NULL)
	{
		perror("failed to allocate memory for array buffer: ");
		return NULL;
	}

	array->index = 0;
	array_allocated_counter++;

	return array;
}

void array_push(array_T *array, void *item)
{
	if (item == NULL) return;

	if (array->buffer == NULL)
	{
		fprintf(stderr, "cannot push element buffer is not allocated.\n");
		return;
	}

	if (array->index >= array->len)
		array->buffer = realloc(array->buffer, (array->len + 10) * array->item_size);

	array->buffer[array->index++] = item;
}

void *array_pop(array_T *array)
{
	if (array->index <= 0) return NULL;
	return array->buffer[--array->index];
}

void *array_get(array_T *array, size_t index)
{
	if (index >= array->index) return NULL;
	return array->buffer[index];
}

array_T *array_take(array_T *array, unsigned int n)
{
	array_T *n_array = init_array(array->item_size);

	for (size_t i = 0; i < n; ++i)
	{
		void *item = array_get(array, i);
		if (item)
			array_push(n_array, item);
	}

	return n_array;
}

array_T *array_drop(array_T *array, unsigned int n)
{
	array_T *n_array = init_array(array->item_size);

	for (size_t i = n; i < array->index; ++i)
	{
		void *item = array_get(array, i);
		if (item)
			array_push(n_array, item);
	}

	return n_array;
}

// TODO(NOTE): it is not well tested, it might have memory leaks.
void array_free(array_T *array)
{
	if (array->buffer)
	{
		free(array->buffer);
		array_allocated_counter--;
	}
}

string_T *init_string(const char *s)
{
	if (s == NULL || s[0] == '\0') return NULL;

	string_T *string = arena_allocate(build_main_arena, sizeof(struct STRING_STRUCT));
	if (string == NULL)
	{
		perror("failed to allocate memory for string: ");
		return NULL;
	}

	string->len = strlen(s);
	string->string = arena_allocate(build_main_arena, string->len);
	if (string->string == NULL)
	{
		perror("failed to allocate memory for string data: ");
		return NULL;
	}

	string->string = strdup(s);

	return string;
}

string_T *string_join(string_T *s0, string_T *s1)
{
	char *s = arena_allocate(build_main_arena, s0->len + s1->len + 1);
	s = strcpy(s, s0->string);
	s = strcat(s, s1->string);

	string_T *string = init_string(s);
	return string;
}

string_T *string_substr(string_T *s, size_t from_pos, size_t to_pos)
{
	if (from_pos >= to_pos) return NULL;

	char *sub = arena_allocate(build_main_arena, (to_pos - from_pos) + 1);
	for (size_t i = 0; i < to_pos - from_pos; ++i)
		sub[i] = s->string[from_pos + i];

	sub[(to_pos - from_pos)] = '\0';
	
	string_T *string = init_string(sub);
	return string;
}

string_T *string_add_seprator(string_T *s, unsigned char seperator, unsigned char replace)
{
	for (size_t i = 0; i < s->len; ++i)
	{
		if (s->string[i] == replace)
			s->string[i] = seperator;
	}

	return s;
}

array_T *string_seperate(string_T *s, unsigned char seperator)
{
	array_T *array = init_array(sizeof(struct STRING_STRUCT));

	size_t pos = 0;
	for (size_t i = 0; i < s->len; ++i)
	{
		if (s->string[i] == seperator)
		{
			string_T *string = string_substr(s, pos, i);
			array_push(array, string);
			pos = i + 1;
		}
	}

	string_T *string = string_substr(s, pos, s->len);
	array_push(array, string);

	return array;
}

string_T *string_from_string_list(const char **s, size_t len)
{
	size_t total_size_of_string = 0;

	for (size_t i = 0; i < len; ++i)
		total_size_of_string += strlen(s[i]) + 1;

	char *string = arena_allocate(build_main_arena, total_size_of_string + 1);
	size_t pos = 0;
	for (size_t i = 0; i < len; ++i)
	{
		string = strcat(string, s[i]);
		pos += strlen(s[i]) + 1;
		string[pos - 1] = ' ';
	}

	string[total_size_of_string] = '\0';

	return init_string(string);
}

string_T *string_from_array(array_T *array)
{
	size_t total_size_of_string = 0;

	for (size_t i = 0; i < array->index; ++i)
	{
		string_T *s = array->buffer[i];
		total_size_of_string += s->len + 1;
	}

	char *string = arena_allocate(build_main_arena, total_size_of_string + 1);
	size_t pos = 0;
	for (size_t i = 0; i < array->index; ++i)
	{
		string_T *s = array->buffer[i];
		string = strcat(string, s->string);
		pos += s->len + 1;
		string[pos - 1] = ' ';
	}

	string[total_size_of_string] = '\0';

	return init_string(string);
}

const char *string_cstr(string_T *s)
{
	return s ? s->string : "";
}

char string_at(string_T *s, size_t i)
{
	if (i < s->len ) return s->string[i];
	return -1;
}

bool string_compare(string_T *s0, string_T *s1)
{
	return !strcmp(s0->string, s1->string);
}

bool string_compare_with_list(string_T *s, array_T *list)
{
	for (size_t i = 0; i < list->index; ++i)
	{
		if (!strcmp(string_cstr(s), string_cstr(array_get(list, i))))
			return true;
	}

	return false;
}

bool command_execute(string_T *command)
{
	if (command == NULL) return false;

	return system(string_cstr(command));
}

array_T *file_get(string_T *path)
{
	bool hash_slash = false;
	char slash = string_cstr(string_substr(path, path->len - 1, path->len))[0];

	if (slash == '/' || slash == '\\')
		hash_slash = true;

	DIR *dir = opendir(string_cstr(path));
	if (dir == NULL)
	{
		perror(string_cstr(formate_string("Directory `%s` does not exist.", path)));
		return NULL;
	}

	array_T *buffer = init_array(sizeof(struct STRING_STRUCT));

	struct dirent *data;
	while ((data = readdir(dir)) != NULL)
	{
#		if __unix__
		if (data->d_type != DT_DIR)
#		elif __WIN32__
		if (data->d_ino != ENOTDIR && (strcmp(data->d_name, ".") && strcmp(data->d_name, "..")))
#		endif
		{
			char *fileName = data->d_name;
			if (hash_slash)
				array_push(buffer, formate_string("%s%s", string_cstr(path), fileName));
			else
#				if __unix__
					array_push(buffer, formate_string("%s/%s", string_cstr(path), fileName));
#				elif __WIN32__
					array_push(buffer, formate_string("%s\\%s", string_cstr(path), fileName));
#				endif
		}
	}

	return buffer;
}

array_T *file_get_end(string_T *path, string_T *end)
{
	array_T *files = file_get(path);
	array_T *files_with_excep = init_array(sizeof(struct STRING_STRUCT));

	for (size_t i = 0; i < files->index; ++i)
	{
		string_T *file = files->buffer[i];
		string_T *substr = string_substr(file, file->len - end->len, file->len);

		if (string_compare(substr, end))
			array_push(files_with_excep, file);
	}

	array_free(files);
	return files_with_excep;
}

bool file_exist(string_T *path)
{
	FILE *file = fopen(string_cstr(path), "r");
	if (file == NULL) return false;

	free(file);
	return true;
}

bool directory_exist(string_T *path)
{
	DIR *dir = opendir(string_cstr(path));
	if (dir == NULL) return false;

	free(dir);
	return true;
}

void create_directory(string_T *path)
{
	size_t pos = 0;
	for (size_t i = 0; i < path->len; ++i)
	{
		if (string_at(path, i) == '/')
		{
			string_T *pre = string_substr(path, 0, pos);
			string_T *sub = string_substr(path, pos, i);

			if (sub)
			{
				string_T *final_dir_path = pre ?
					formate_string("mkdir %s%s", string_cstr(pre), string_cstr(sub)) :
					formate_string("mkdir %s", string_cstr(sub));

				if (!directory_exist(string_substr(final_dir_path, 6, final_dir_path->len)))
					command_execute(final_dir_path);
			}

			pos = i + 1;
		}
	}

	string_T *pre = string_substr(path, 0, pos);
	string_T *sub = string_substr(path, pos, path->len);
	if (sub)
	{
		string_T *final_dir_path = pre ?
			formate_string("mkdir %s%s", string_cstr(pre), string_cstr(sub)) :
			formate_string("mkdir %s", string_cstr(sub));

		if (!directory_exist(string_substr(final_dir_path, 6, final_dir_path->len)))
			command_execute(final_dir_path);
	}
}

time_t get_time_from_file(string_T *path)
{
	struct stat file_stat;
	if (stat(string_cstr(path), &file_stat) == -1) {
		perror(string_cstr(formate_string("Failed to get file status: %s, ", path)));
		return (time_t)(-1);  // Return -1 on error
	}

	return file_stat.st_mtime;
}

bool binary_test(string_T *binary_path, array_T *source_list)
{
	time_t binary_timestamp = get_time_from_file(binary_path);
	if (binary_timestamp == (time_t)(-1))
		return true;

	for (size_t i = 0; i < source_list->index; ++i) {
		time_t source_timestamp = get_time_from_file(array_get(source_list, i));
		if (source_timestamp == (time_t)(-1)) {
			fprintf(
					stderr,
					"Failed to get modification time for source file: %s\n",
					string_cstr(array_get(source_list, i))
			);
			continue;
		}

		if (source_timestamp > binary_timestamp)
			return true;
	}

	return false;
}

void exit_build_system(int status_code)
{
	if (array_allocated_counter > 0)
		WARN("You haven't freed %d array, which may result in memory leaks.", array_allocated_counter);

	free(build_main_arena->buffer);
	free(build_main_arena);

	exit(status_code);
}

// main entry
int main(int argc, char **argv)
{
	build_main_arena = init_arena(8 * 1024 * 1024); // allocating 8 mb, (hopefully it will be enough)
	
	string_T *files_string = formate_string("%s build.h", build_source);
	array_T *files = string_seperate(files_string, ' ');

	if (binary_test(init_string(build_bin), files))
	{
		bool status = command_execute(formate_string("gcc %s -I. -o %s", build_source, build_bin));
		if (!status) INFO("sucessfully updated binary of build system. Re-run to see the changes.");
		else ERROR("failed to update the binary of build system.");

		return 0;
	}

	array_free(files);

	build_start(argc, argv);

	exit_build_system(0);
}

#endif // IMPLEMENT_BUILD_H
