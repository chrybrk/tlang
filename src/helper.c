#include "include/helper.h"

const char *token_type_as_string(token_type_T at)
{
	const char *type;

	switch (at)
	{
		case TT_ID: type = "TT_ID"; break;
		case TT_INT: type = "TT_INT"; break;
		case TT_STAR: type = "TT_STAR"; break;
		case TT_FSLASH: type = "TT_FSLASH"; break;
		case TT_BSLASH: type = "TT_BSLASH"; break;
		case TT_PLUS: type = "TT_PLUS"; break;
		case TT_MINUS: type = "TT_MINUS"; break;
		case TT_COLON: type = "TT_COLON"; break;
		case TT_SEMI: type = "TT_SEMI"; break;
		case TT_LP: type = "TT_LP"; break;
		case TT_RP: type = "TT_RP"; break;
		case TT_ASSIGN: type = "TT_ASSIGN"; break;
		case TT_EOF: type = "end"; break;
		default: type = "not-implemented";
	}

	return type;
}

void print_token(token_T *token)
{
	if (!token) return;

	const char *type = token_type_as_string(token->type);

	printf("Token(%s, %s, %ld:%ld:%d)\n",
			type,
			token->value,
			token->position.line,
			token->position.column,
			token->position.length
	);
}

const char *ast_type_as_string(ast_type_T at)
{
	const char *type;

	switch (at)
	{
		case AST_LET: type = "AST_LET"; break;
		case AST_CALL: type = "AST_CALL"; break;
		case AST_PRIMARY: type = "AST_PRIMARY"; break;
		case AST_FACTOR: type = "AST_FACTOR"; break;
		case AST_TERM: type = "AST_TERM"; break;
		case AST_EXPR: type = "AST_EXPR"; break;
		case AST_LIST: type = "AST_LIST"; break;
		case AST_STMT: type = "AST_STMT"; break;
		case AST_PRGM: type = "AST_PRGM"; break;
		default: type = "not-implemented";
	}

	return type;
}

void print_ast_as_tree(ast_T *ast)
{
	if (!ast) return;

	const char *type = ast_type_as_string(ast->type);

	if (ast->token)
	{
		printf("AST(%s) - ", type);
		print_token(ast->token);
	}
	else printf("AST(%s)\n", type);

	print_ast_as_tree(ast->left);
	print_ast_as_tree(ast->mid);
	print_ast_as_tree(ast->right);
}
