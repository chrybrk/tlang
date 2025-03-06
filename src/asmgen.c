#include "asmgen.h"

static char *section_text = NULL;
static char *section_data = NULL;
static FILE *OUTPUT = NULL;

static const char *r64[] = { "rax", "rbx", "rcx", "rdx" };
static const char *r32[] = { "eax", "ebx", "ecx", "edx" };
static const char *r16[] = { "ax", "bx", "cx", "dx" };
static const char *r8[] = { "al", "bl", "cl", "dl" };
static bool reg_free[4] = { true };

static char **FROM = NULL;
static int		REG_ID = -1;

const char *get_reg(const char *from[4])
{
	for (int i = 0; i < 4; ++i)
	{
		if (reg_free[i])
		{
			reg_free[i] = false;
			REG_ID = i;
			FROM = NULL;
			return from[i];
		}
	}

	return NULL;
}

void free_reg()
{
	REG_ID = -1;
	FROM = NULL;
	for (int i = 0; i < 4; ++i) reg_free[i] = true;
}

const char *data_type_to_data_directive(data_type_T data_type, bool is_reserved)
{
	switch (data_type)
	{
		case dchar:
		case di8:
		case du8: 	return is_reserved ? "rb" : "db";
		case di16:
		case du16: 	return is_reserved ? "rw" : "dw";
		case di32:
		case df32:
		case du32:	return is_reserved ? "rd" : "dd";
		case di64:
		case du64:
		case df64:
		case dstr: 	return is_reserved ? "rq" : "dq";
		default:
		{
			printf("err :: no reserved data directives for this type.\n");
			return NULL;
		}
	}
}

const char *expr_ast_type_to_symb(ast_type_T type)
{
	switch (type)
	{
		case ast_add: return "+";
		case ast_sub: return "-";
		case ast_mul: return "*";
		case ast_div: return "/";
		default: return "";
	}
}

const char *expr_ast_type_to_ins(ast_type_T type)
{
	switch (type)
	{
		case ast_add: return "add";
		case ast_sub: return "sub";
		case ast_mul: return "imul";
		case ast_div: return "idiv";
		default: return "";
	}
}

const char **get_reg_list(data_type_T dt)
{
	switch (dt)
	{
		case dchar:
		case di8:
		case du8: 	return r8;
		case di16:
		case du16:	return r16;
		case di32:
		case du32:
		case df32:	return r32;
		case di64:
		case du64:
		case df64:
		case dstr: 	return r64;
		default: return NULL;
	}
}

const char *expr(ast_T *root)
{
	if (root->type == ast_const) return root->token->value;
	else if (root->type == ast_ident)
	{
		const char *r = get_reg(get_reg_list(root->data_type));
		section_text =
			strjoin(section_text, formate_string("\tmov \t%s, [%s]\n", r, root->token->value));
		return r;
	}
	else
	{
		if (root->left->type == ast_const && root->right->type == ast_const)
			return formate_string("%s %s %s", expr(root->left), expr_ast_type_to_symb(root->type), expr(root->right));
		else
		{
			const char *r;
			if (root->left->type != ast_const)
			{
				r = expr(root->left);
				const char *s1 = formate_string("\t%s \t%s, %s\n",
					expr_ast_type_to_ins(root->type),
					r, expr(root->right)
				);
				section_text = strjoin(section_text, s1);
			}
			else
			{
				r = expr(root->right);
				const char *s1 = formate_string("\t%s \t%s, %s\n",
					expr_ast_type_to_ins(root->type),
					r, expr(root->left)
				);
				section_text = strjoin(section_text, s1);
			}

			return r;
		}
	}
}

void assign(ast_T *root)
{
	const char *s1;
	if (root->left->type != ast_const)
	{
		s1 = formate_string("%s %s 0\n",
			root->token->value,
			data_type_to_data_directive(root->data_type, false)
		);

		const char *r = expr(root->left);
		uint8_t lhs = get_data_type_size(root->data_type),
						rhs = get_data_type_size(root->left->data_type);
		const char *txt = formate_string("\tmov \t[%s], %s\n",
			root->token->value,
			(lhs > 0 && rhs > 0) ?
				get_reg_list(root->data_type)[REG_ID] :
				r
		);

		section_text = strjoin(section_text, txt);
	}
	else
	{
		s1 = formate_string("%s %s %s\n",
			root->token->value,
			data_type_to_data_directive(root->data_type, false),
			root->left->token->value
		);
	}

	section_data = strjoin(section_data, s1);
	free_reg();
}

void at_asm(ast_T *root)
{
	section_text = strjoin(section_text, 
		formate_string("\t%s\n", root->token->value)
	);
}

void statement(ast_T *root)
{
	switch (root->type)
	{
		case ast_join:
			statement(root->left);
			statement(root->right);
			break;

		case ast_assign:
			assign(root);
			break;

		case ast_at_asm:
			at_asm(root);
			break;

		default:
			break;
	}
}

void init_asmgen(const char *output, ast_T *root)
{
	if (!output) return;

	OUTPUT = fopen(output, "w");

	section_text = formate_string(
		"section '.text' executable\n"
		"extrn putchar\n"
		"extrn exit\n"
		"public _start\n"
		"_start:\n"
	);
	section_data = formate_string("section '.data' writeable\n");
	statement(root);
	fprintf(OUTPUT,"format ELF64\n%s%s", section_text, section_data);
}
