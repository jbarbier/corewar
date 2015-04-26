#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

char* file_to_memory(char* file_name)
{
	char*	data = NULL;
	int		fd;
	int		size;

	fd = _open(file_name, OPEN_MODE);
	if (fd >= 0)
	{
		_lseek(fd, 0, SEEK_SET);
		size = _lseek(fd, 0, SEEK_END);

		data = malloc(size);
		_lseek(fd, 0, SEEK_SET);
		_read(fd, data, size);
		_close(fd);
	}

	return data;
}
