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

void			ring_buffer_add(t_ring_buffer* ring, void* data)
{
	if (ring->write_index >= ring->size)
	{
		ring->free_element_fct(ring->data[ring->read_index % ring->size]);
		ring->read_index++;
	}
	
	ring->data[ring->write_index % ring->size] = data;
	ring->write_index++;
}

void			ring_buffer_destroy(t_ring_buffer* ring)
{
	int count = ring->size;
	int start = ring->read_index;
	while (count--)
	{
		ring->free_element_fct(ring->data[start % ring->size]);
		start++;
	}

	free(ring->data);
	free(ring);
}

t_ring_buffer* ring_buffer_initialize(int size, t_ring_fct_free free_element_fct)
{
	t_ring_buffer* ring = (t_ring_buffer*)malloc(sizeof(t_ring_buffer));
	ring->size = size;
	ring->read_index = 0;
	ring->write_index = 0;
	ring->data = malloc(sizeof(void*) * size);
	memset(ring->data, 0, sizeof(void*) * size);
	ring->free_element_fct = free_element_fct;
	return ring;
}

void**			ring_buffer_get_last(t_ring_buffer* ring)
{
	int index;
	if (ring->write_index == 0)
		return NULL;
	index = (ring->write_index - 1);
	if (index < 0)
		index = ring->size - 1;
	return &ring->data[index % ring->size];
}