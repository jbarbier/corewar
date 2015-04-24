#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

typedef struct s_memory
{
	int8*			data;
	int32			size;
} t_memory;

t_memory*	memory_initialize(int32 size);
void		memory_destroy(t_memory* memory);


#endif
