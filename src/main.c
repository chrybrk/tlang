#include <stdio.h>
#include <string.h>
#include "include/lexer.h"
#include "include/parser.h"
#include "include/helper.h"
#include "include/utils.h"

int main(int argc, const char **argv)
{
	if (argc < 2) printf("tlang: no input file.\n"), exit(0);

	const char *source = read_file(argv[1]);

	lexer_T *lexer = init_lexer(source);

	parser_T *parser = init_parser(lexer);
	ast_T *root = parser_parse(parser);
	print_ast_as_tree(root);

	return 0;
}
