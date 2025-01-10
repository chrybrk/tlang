#define IMPLEMENT_BUILD_H
#include "build.h"

const char *build_source = "build.c";
const char *build_bin = "build";

void build_start(int argc, char **argv)
{
	// create bin folder if it does not exist.
	create_directory(init_string("bin"));

	// capture all the source files
	// into an array
	string_T *source_path = init_string("src/");
	string_T *source_include = init_string("-Isrc/ -Isrc/include");
	string_T *bin = init_string("bin/tlang");

	array_T *source_files = file_get_end(source_path, init_string(".c"));

	// compile
	bool status = command_execute(
			formate_string("gcc %s %s -o %s",
				string_cstr(string_from_array(source_files)),
				string_cstr(source_include),
				string_cstr(bin)
			)
	);
	array_free(source_files);

	string_T *args = string_from_string_list((const char **)argv, argc);

	array_T *args_array = string_seperate(args, ' ');
	array_T *first_arg = array_drop(args_array, 1);

	array_free(args_array);

	if (!status)
	{
		(void)command_execute(
				formate_string("exec %s %s",
					string_cstr(bin),
					string_cstr(string_from_array(first_arg))
				)
			);
		array_free(first_arg);
	}
	else ERROR("failed to compile.");
}
