#include <stdio.h>
#include <string.h>
#include "include/lexer.h"
#include "include/helper.h"
#include "include/utils.h"

int main(int argc, const char **argv)
{
	if (argc < 2) printf("tlang: no input file.\n"), exit(0);

	const char *source = read_file(argv[1]);
	lexer_T *lexer = init_lexer(source);

	token_T *tok;
	while ((tok = next_token(lexer))->type != TT_EOF)
		print_token(tok);

	return 0;
}
