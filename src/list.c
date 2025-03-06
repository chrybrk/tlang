#include "list.h"

list_T *init_list(size_t item_size)
{
	list_T *list = malloc(sizeof(struct LIST_STRUCT));
	if (!list)
	{
		perror("err :: failed to allocate memory for list: ");
		return NULL;
	}

	list->item_size = item_size;
	list->index = 0;
	list->buffer_size = 15;
	list->buffer = malloc(item_size * list->buffer_size);
	if (!list->buffer)
	{
		perror("err :: failed to allocate memory for list->buffer: ");
		return NULL;
	}

	return list;
}

void list_push(list_T *list, void *item)
{
	if (!item) return;

	if (list->index >= list->buffer_size)
	{
		list->buffer_size = (list->index + 15);
		list->buffer = realloc(
			list->buffer, list->buffer_size * list->item_size
		);

		if (!list->buffer)
		{
			printf("warn :: trying to allocate `%ld` bytes.\n",
				list->item_size * list->buffer_size
			);
			perror("err :: failed to allocate memory for list->buffer: ");
			return;
		}
	}

	list->buffer[list->index++] = item;
}

void *list_pop(list_T *list)
{
	if (list->index > 0)
		return list->buffer[--list->index];
	return NULL;
}

void *list_get(list_T *list, size_t index)
{
	if (index < list->index)
		return list->buffer[index];
	return NULL;
}

size_t list_length(list_T *list)
{
	return list->index;
}

void list_extend(list_T *listA, list_T *listB)
{
	for (size_t i = 0; i < listB->index; ++i)
		list_push(listA, list_get(listB, i));
}

void list_free(list_T *list)
{
	free(list->buffer);
	free(list);
}
