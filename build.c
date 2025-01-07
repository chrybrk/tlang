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

	if (!status)
		(void)command_execute(formate_string("exec %s", string_cstr(bin)));
	else ERROR("failed to compile.");
}
