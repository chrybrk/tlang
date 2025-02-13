#include <stdio.h>
#include "lex.h"
#include "parser.h"
#include "glob.h"

hash_T *symb_table;

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "no file.\n");
		return -1;
	}

	symb_table = init_hash();

	lexer_T *lexer = init_lexer(argv[1]);
	list_T *tokens = lexer_get_tokens(lexer);

	printf("\n\n--------------------------\n\n");

	for (ssize_t i = 0; i < list_length(tokens); ++i)
	{
		token_T *token = list_get(tokens, i);
		printf("Token(%s, %s)\n", token_to_string(token->type), token->value);
	}

	printf("\n\n--------------------------\n\n");

	parser_T *parser = init_parser(tokens);
	ast_T *root = parser_parse_program(parser);
	pretty_ast_tree(root, 0);

	printf("\n\n--------------------------\n\n");

	return 0;
}
