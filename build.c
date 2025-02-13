#define IMPLEMENT_BUILD_H
#include "build.h"

#define PROJECT_NAME 			"tlang"
#define PROJECT_BIN				"bin/"
#define PROJECT_SRC				"src/"
#define PROJECT_INCLUDE		"src/"

const char *build_source = "build.c";
const char *build_bin = "build";

void build_start(int argc, char **argv)
{
	// create bin folder if it does not exist.
	create_directory("bin");

	array_T *src_files_array = file_get_end(PROJECT_SRC, ".c");
	const char *src_files = strconvCAtoCC(src_files_array, ' ');
	array_free(src_files_array);

	command_execute(
			formate_string(
				"gcc -I%s %s -o %s%s",
				PROJECT_INCLUDE,
				src_files,
				PROJECT_BIN,
				PROJECT_NAME
			)
	);
}
