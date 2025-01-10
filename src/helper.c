#include "include/lexer.h"

void print_token(token_T *token)
{
	const char *type;

	switch (token->type)
	{
		case TT_ID: type = "TT_ID"; break;
		case TT_INT: type = "TT_INT"; break;
		case TT_COLON: type = "TT_COLON"; break;
		case TT_SEMI: type = "TT_SEMI"; break;
		case TT_LP: type = "TT_LP"; break;
		case TT_RP: type = "TT_RP"; break;
		case TT_EQ: type = "TT_EQ"; break;
		case TT_EOF: type = "end"; break;
	}

	printf("Token(%s, %s, %ld:%ld:%d)\n",
			type,
			token->value,
			token->position.line,
			token->position.column,
			token->position.length
	);
}
