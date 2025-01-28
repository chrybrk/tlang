#include "include/helper.h"

const char *token_type_as_string(token_type_T at)
{
	const char *type;

	switch (at)
	{
		case TT_ID: type = "identifier"; break;
		case TT_INT: type = "integer"; break;
		case TT_STRING: type = "string"; break;
		case TT_STAR: type = "*"; break;
		case TT_FSLASH: type = "/"; break;
		case TT_BSLASH: type = "\\"; break;
		case TT_PLUS: type = "+"; break;
		case TT_MINUS: type = "-"; break;
		case TT_COLON: type = ":"; break;
		case TT_COMMA: type = ","; break;
		case TT_SEMI: type = ";"; break;
		case TT_LP: type = "("; break;
		case TT_RP: type = ")"; break;
		case TT_LS: type = "["; break;
		case TT_RS: type = "]"; break;
		case TT_LB: type = "{"; break;
		case TT_RB: type = "}"; break;
		case TT_ASSIGN: type = "="; break;
		case TT_EOF: type = "(eof)"; break;
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
		case AST_RETURN: type = "AST_RETURN"; break;
		case AST_ASSIGN: type = "AST_ASSIGN"; break;
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

void print_ast_as_tree(ast_T *ast, int level)
{
	if (!ast) return;

	const char *type = ast_type_as_string(ast->type);

	for (int i = 0; i < level; ++i)
		printf("│");

	printf("└");

	if (ast->token)
	{
		printf("%s - (%s, %s)\n", type, token_type_as_string(ast->token->type), ast->token->value);
	}
	else printf("AST(%s)\n", type);

	print_ast_as_tree(ast->left, level + 1);
	print_ast_as_tree(ast->mid, level + 1);
	print_ast_as_tree(ast->right, level + 1);
}

int isnotdq(int c)
{
	if (c == '"') return 1;
	return 0;
}

int isident(int c)
{
	if (isalnum(c) || c == '_') return 1;
	return 0;
}
