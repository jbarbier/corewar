#include <stdlib.h>
#include <memory.h>

#include "memory.h"

void		memory_flag_reset(t_memory* memory)
{
	memset(memory->flag, 0, memory->size);
}


t_memory*	memory_initialize(int size)
{
	t_memory* mem = (t_memory*)malloc(sizeof(t_memory));
	mem->data = malloc(size);
	mem->flag = malloc(size);
	mem->size = size;	
	return mem;
}

void		memory_destroy(t_memory* memory)
{
	free(memory->data);
	free(memory);
}

