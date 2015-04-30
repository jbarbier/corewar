#ifndef UTILS_H
#define UTILS_H

#	include <fcntl.h>
#if defined(_WIN32)
#	include <io.h>
#	define OPEN_MODE O_RDONLY | O_BINARY
#else
#	define OPEN_MODE O_RDONLY
#	define _open open
#	define _read read
#	define _close close
#	define _lseek lseek
#	define _strdup strdup;
#endif

char* file_to_memory(char* file_name, int* size);


typedef void(*t_ring_fct_free)(void*);
struct s_ring_buffer
{
	int					size;
	int					read_index;
	int					write_index;
	void**				data;
	t_ring_fct_free		free_element_fct;
	
};
typedef struct s_ring_buffer t_ring_buffer;

t_ring_buffer*	ring_buffer_initialize(int size, t_ring_fct_free free_element_fct);
void			ring_buffer_destroy(t_ring_buffer* ring);
void			ring_buffer_add(t_ring_buffer* ring, void* data);
void**			ring_buffer_get_last(t_ring_buffer* ring);


#endif