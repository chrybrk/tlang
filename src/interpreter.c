#include "include/interpreter.h"
#include "include/helper.h"

int program(ast_T *root);

int number(ast_T *root)
{
	return root ? root->token ? atoi(root->token->value) : -1 : -139;
}

int factor(ast_T *root)
{
	int left = program(root->left);

	return left;
}

int term(ast_T *root)
{
	int left = program(root->left);

	if (root->token && (root->token->type == TT_STAR || root->token->type == TT_FSLASH))
	{
		int right = program(root->right);

		if (root->token->type == TT_STAR) left *= right;
		else left /= right;
	}

	return left;
}

int expr(ast_T *root)
{
	int left = program(root->left);

	if (root->token && (root->token->type == TT_PLUS || root->token->type == TT_MINUS))
	{
		int right = program(root->right);

		if (root->token->type == TT_PLUS) left += right;
		else left -= right;
	}

	return left;
}

int program(ast_T *root)
{
	switch (root->type)
	{
		case AST_PRGM: return program(root->left);
		case AST_EXPR: return expr(root);
		case AST_TERM: return term(root);
		case AST_FACTOR: return factor(root);
		case AST_NUMBER: return number(root);
	}

	return 0;
}

int interpreter_interpret(ast_T *root)
{
	if (!root) return 0;

	return program(root);
}
