#include "include/utils.h"

const char *read_file(const char *filename)
{
	FILE* file = fopen(filename, "rb");
	if (file == NULL)
	{
		perror("could not open file: ");
		return NULL;
	}

	ssize_t len;

	fseek(file, 0L, SEEK_END);
	len = ftell(file);
	fseek(file, 0L, SEEK_SET);

	char *buffer = calloc(1, sizeof(char) * len);
	fread(buffer, sizeof(char), len, file);

	fclose(file);

	buffer[len - 1] = '\0';

	return buffer;
}
