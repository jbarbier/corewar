#include <stdlib.h>
#include <memory.h>

#include "memory.h"

t_memory*	memory_initialize(int size)
{
	t_memory* mem = (t_memory*)malloc(sizeof(t_memory));
	mem->data = malloc(size);
	memset(mem->data, 0, size);
	mem->size = size;	
	return mem;
}

void		memory_destroy(t_memory* memory)
{
	free(memory->data);
	free(memory);
}

