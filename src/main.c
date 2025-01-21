#include <stdio.h>
#include <string.h>
#include "include/global.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/helper.h"
#include "include/utils.h"

const char *filename;

// TODO: generate proper errors
// TODO: assign variables, and re-assign when asked for.

int main(int argc, const char **argv)
{
	if (argc < 2) printf("tlang: no input file.\n"), exit(0);

	filename = argv[1];

	const char *source = read_file(filename);

	lexer_T *lexer = init_lexer(source);

	parser_T *parser = init_parser(lexer);
	ast_T *root = parser_parse(parser);
	print_ast_as_tree(root);

	return 0;
}
