#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

#include "core.h"

int32 be_to_le_int32(int32 value)
{
	int32 ret;
	int8* ptr = (int8*)&ret;

	*ptr++ = (int8)((value >> 24) & 0xff);
	*ptr++ = (int8)((value >> 16) & 0xff);
	*ptr++ = (int8)((value >> 8) & 0xff);
	*ptr = (int8)((value)& 0xff);

	return ret;
}

int8* core_get_code_ptr(struct s_core* core)
{
	return (int8*)(core)+sizeof(core->header);
}

t_core* core_load_from_file(const char* file_name)
{
	int fd = _open(file_name, O_RDONLY | O_BINARY);

	if (fd < 0)
		return NULL;

	t_core* core = NULL;

	int magic;
	if (_read(fd, &magic, 4) == 4)
	{
		int* core_magic = (int*)(COREWAR_EXEC_MAGIC);
		if (magic == *core_magic)
		{
			long size;
			int8* data;

			_lseek(fd, 0, SEEK_SET);
			size = _lseek(fd, 0, SEEK_END);

			data = malloc(size);
			_lseek(fd, 0, SEEK_SET);
			_read(fd, data, size);

			core = (t_core*)data;
			core->header.prog_size = be_to_le_int32(core->header.prog_size);
		}
	}

	_close(fd);
	return  core;
}