#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "asmgen.h"
#include "glob.h"

trie_node_T *token_trie_map;
trie_node_T *symbol_trie_map;
uint64_t GLOBAL_INDEX;
uint64_t LOCAL_INDEX;
uint64_t SYMBOL_SIZE;
symbol_T *SYMBOLS;

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "no file.\n");
		return -1;
	}

	token_trie_map = init_trie_node();
	trie_insert(token_trie_map, "import", (trie_value_T){ .value.i32 = tt_import });
	trie_insert(token_trie_map, "return", (trie_value_T){ .value.i32 = tt_return });
	trie_insert(token_trie_map, "if", 		(trie_value_T){ .value.i32 = tt_if });
	trie_insert(token_trie_map, "else", 	(trie_value_T){ .value.i32 = tt_else });
	trie_insert(token_trie_map, "void", 	(trie_value_T){ .value.i32 = tt_void });
	trie_insert(token_trie_map, "char", 	(trie_value_T){ .value.i32 = tt_char });
	trie_insert(token_trie_map, "str", 		(trie_value_T){ .value.i32 = tt_str });
	trie_insert(token_trie_map, "i8", 		(trie_value_T){ .value.i32 = tt_i8 });
	trie_insert(token_trie_map, "i16", 		(trie_value_T){ .value.i32 = tt_i16 });
	trie_insert(token_trie_map, "i32", 		(trie_value_T){ .value.i32 = tt_i32 });
	trie_insert(token_trie_map, "i64", 		(trie_value_T){ .value.i32 = tt_i64 });
	trie_insert(token_trie_map, "u8", 		(trie_value_T){ .value.i32 = tt_u8 });
	trie_insert(token_trie_map, "u16", 		(trie_value_T){ .value.i32 = tt_u16 });
	trie_insert(token_trie_map, "u32", 		(trie_value_T){ .value.i32 = tt_u32 });
	trie_insert(token_trie_map, "u64",		(trie_value_T){ .value.i32 = tt_u64 });
	trie_insert(token_trie_map, "f32",		(trie_value_T){ .value.i32 = tt_f32 });
	trie_insert(token_trie_map, "f64",		(trie_value_T){ .value.i32 = tt_f64 });

	symbol_trie_map = init_trie_node();
	SYMBOL_SIZE = 1024;
	GLOBAL_INDEX = 0;
	LOCAL_INDEX = SYMBOL_SIZE - 1;
	SYMBOLS = malloc(sizeof(struct SYMBOL_STRUCT) * SYMBOL_SIZE);
	if (!SYMBOLS)
	{
		perror("err :: failed to allocate memory for symbols: ");
		return -1;
	}

	lexer_T *lexer = init_lexer(argv[1]);
	list_T *tokens = lexer_get_tokens(lexer);

	// printf("\n\n--------------------------\n\n");

	/*
	for (ssize_t i = 0; i < list_length(tokens); ++i)
	{
		token_T *token = list_get(tokens, i);
		printf("Token(%s, %s)\n", token_type_to_string(token->type), token->value);
	}
	*/

	// printf("\n\n--------------------------\n\n");

	parser_T *parser = init_parser(tokens);
	ast_T *root = parser_parse(parser);
	// pretty_ast_tree(root, 0);

	// printf("\n\n--------------------------\n\n");

	init_asmgen("out.asm", root);

	return 0;
}
