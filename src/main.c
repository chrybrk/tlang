#include <stdio.h>
#include <string.h>
#include "include/lexer.h"
#include "include/utils.h"

int main(void)
{
	const char *source = read_file("./example/input000.tlang");
	lexer_T *lexer = init_lexer(source, strlen(source));

	token_T *tok;
	while ((tok = next_token(lexer))->type != TT_EOF)
	{
		token_show(tok);
	}

	return 0;
}
