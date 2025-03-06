#ifndef __list_h__
#define __list_h__

#include <stdio.h>
#include <stdlib.h>

typedef struct LIST_STRUCT {
	void **buffer;
	size_t buffer_size,
				 index,
				 item_size;
} list_T;

list_T *init_list(size_t item_size);
void list_push(list_T *list, void *item);
void *list_pop(list_T *list);
void *list_get(list_T *list, size_t index);
size_t list_length(list_T *list);
void list_extend(list_T *listA, list_T *listB);
void list_free(list_T *list);

#endif // __list_h__
