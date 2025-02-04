#define IMPLEMENT_BUILD_H
#include "build.h"

const char *build_source = "build.c";
const char *build_bin = "build";

void build_start(int argc, char **argv)
{
	// create bin folder if it does not exist.
	create_directory("bin");

	// define project
	const char *project_bin = "bin/tlang";
	const char *project_source_path = "src/";
	const char *project_includes = "-Isrc/include";

	array_T *project_source_files_array = file_get_end(project_source_path, ".c");
	const char *project_source_files = strconvCAtoCC(project_source_files_array, ' ');

	array_free(project_source_files_array);

	// build
	int project_compile_status = command_execute(
			formate_string(
				"gcc %s %s -o %s",
				project_includes,
				project_source_files,
				project_bin
			)
	);

	if (!project_compile_status)
	{
		const char *argv_cc = strconvCCAtoCC((const char **)argv, argc, ' ');
		array_T *argv_array = strtoarray(argv_cc, ' ');

		if (argc >= 2)
			command_execute(formate_string("exec %s %s", project_bin, (const char *)array_get(argv_array, 1)));

		array_free(argv_array);
	}
	else ERROR("failed to compile.");
}
