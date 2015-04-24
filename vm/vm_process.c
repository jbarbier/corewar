#include <stdlib.h>
#include <memory.h>
#include "vm.h"
#include "core.h"
#include "op.h"

t_process*	vm_create_process(t_vm* vm, t_process* parent, int pc)
{
	t_process* process = NULL;
	if (vm->process_pool_count > 0)
		process = vm->processes_pool[--vm->process_pool_count];
	else
		process = malloc(sizeof(t_process));

	if (parent)
		memcpy(process, parent, sizeof(t_process));
	else
		memset(process, 0, sizeof(t_process));

	process->pc = pc;
	process->internal_id = vm->process_counter++;

	vm_get_opcode(vm, process);
	vm->processes[vm->process_count++] = process;

	return process;
}


void vm_clean_dead_process(t_vm* vm)
{
	int32 current = 0;
	int32 previous = 0;


	for (; previous < vm->process_count; ++previous)
	{
		if (vm->processes[previous]->free == 0)
			vm->processes[current++] = vm->processes[previous];
	}
	vm->process_count = current;
}

void vm_destroy_process(t_vm* vm, t_process* process)
{
	vm->processes_pool[vm->process_pool_count++] = process;
	process->free = 1;
}

t_process*	vm_add_core(t_vm* vm, struct s_core* core, int32 core_id, int32 offset)
{

	int8* core_code = core_get_code_ptr(core);
	vm->write_copy(vm, offset, core_code, core->header.prog_size);
	t_process* process = vm_create_process(vm, NULL, offset);
	process->reg[0] = core_id;
	process->core_id = core_id;
	return process;
}

void	vm_kill_process_if_no_live(t_vm* vm)
{
	for (int i = 0; i < vm->process_count; ++i)
	{
		if (vm->processes[i]->cycle_live == 0)
			vm_destroy_process(vm, vm->processes[i]);
		else
			vm->processes[i]->cycle_live = 0;
	}
}