#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "utils.h"

char* file_to_memory(char* file_name, int* size_output)
{
	char*	data = NULL;
	int		fd;
	int		size = 0;

	fd = _open(file_name, OPEN_MODE);
	if (fd >= 0)
	{
		_lseek(fd, 0, SEEK_SET);
		size = _lseek(fd, 0, SEEK_END);

		data = malloc(size + 1);
		memset(data, 0, size + 1);
		_lseek(fd, 0, SEEK_SET);
		_read(fd, data, size);
		_close(fd);
	}

	if (size_output)
		*size_output = size;

	return data;
}
