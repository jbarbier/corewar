#include <stdlib.h>
#include <memory.h>
#include "vm.h"
#include "core.h"
#include "op.h"

t_process*	vm_create_process(t_vm* vm, t_process* parent, int32 pc)
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
	process->next_pc = pc;
	process->internal_id = vm->process_counter++;
	process->cycle_live = vm->cycle_current;
	process->memory_write_op_count = 0;
	process->memory_read_op_count = 0;
	process->cycle_wait = 0;	
	vm->processes[vm->process_count++] = process;
	vm_get_opcode(vm, process);
	return process;
}

void	vm_reset_process_io_op(t_process* process)
{
	process->memory_read_op_count = 0;
	process->memory_write_op_count = 0;
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
	t_process* process = NULL;

	if (vm->core_count < VM_MAX_CORES)
	{
		vm->write_copy(vm, offset, core_code, core->header->prog_size);
		process = vm_create_process(vm, NULL, offset);
		process->reg[0] = core_id;
		process->core_id = core_id;
		core->id = core_id;
		vm->cores[vm->core_count++] = core;
	}

	return process;
}

void	vm_kill_process_if_no_live(t_vm* vm)
{
	int32 i = 0;
	for (; i < vm->process_count; ++i)
	{
		if (vm->processes[i]->cycle_live == 0)
		{			
			vm_destroy_process(vm, vm->processes[i]);
		}
		else
			vm->processes[i]->cycle_live = 0;
	}
}
