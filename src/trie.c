#include "trie.h"

trie_node_T *init_trie_node()
{
	trie_node_T *node = malloc(sizeof(struct TRIE_STRUCT));
	if (!node)
	{
		perror("err :: failed to allocate memory for trie node: ");
		return NULL;
	}

	node->children = malloc(sizeof(struct TRIE_STRUCT) * 256);
	if (!node->children)
	{
		perror("err :: failed to allocate memory for trie node->children: ");
		return NULL;
	}
	
	return node;
}

void trie_insert(trie_node_T *node, const char *key, trie_value_T value)
{
	for (size_t i = 0; i < strlen(key); ++i)
	{
		if (node->children[key[i]] == NULL)
			node->children[key[i]] = init_trie_node();
		node = node->children[key[i]];
	}
	node->value = value;
	node->is_terminal = true;
}

trie_value_T trie_find(trie_node_T *node, const char *key)
{
	for (size_t i = 0; i < strlen(key); ++i)
	{
		if (node->children[key[i]] == NULL)
			return (trie_value_T) { .is_value = false };
		node = node->children[key[i]];
	}
	node->value.is_value = true;
	return node->value;
}
