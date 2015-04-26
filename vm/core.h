#ifndef CORE_H
#define CORE_H

#include "types.h"
#include "op.h"


typedef struct s_core
{
	t_header*	header;
	int			id;
	int			live_count;
} t_core;

int8*	core_get_code_ptr(struct s_core* core);
t_core* core_load_from_file(const char* file_name);
void	core_destroy(t_core* core);

#endif