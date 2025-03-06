#ifndef __trie_h__
#define __trie_h__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
	union {
		char i8;
		short i16;
		int i32;
		void *ptr;
	} value;
	bool is_value;
} trie_value_T;

typedef struct TRIE_STRUCT trie_node_T;

typedef struct TRIE_STRUCT {
	trie_node_T **children;
	bool is_terminal;
	trie_value_T value;
} trie_node_T;

trie_node_T *init_trie_node();
void trie_insert(trie_node_T *node, const char *key, trie_value_T value);
trie_value_T trie_find(trie_node_T *node, const char *key);

#endif // __trie_h__
