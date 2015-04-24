#include <stdlib.h>

#include "memory.h"
#include "core.h"
#include "vm.h"




int main(int ac, char** av)
{
	t_core*		core	= core_load_from_file(av[1]);
	t_vm*		vm		= vm_initialize();
	t_process*	process = (t_process*) malloc(sizeof(t_process));
	
	vm_add_core(vm, core, 0xcacacaca, 0);

	while (vm->process_count)
	{				
		vm->cycle_current++;
		for (int i = 0; i < vm->process_count; ++i)
		{
			t_process* process = vm->processes[i];
			if (process->cycle_wait == 0)
			{
				vm_execute(vm, process);
				vm_get_opcode(vm, process);

				if (vm->live_count > NBR_LIVE)
				{
					vm->live_count = 0;
					vm->cycle_to_die -= vm->cycle_delta;
				}					
			}
			else
				process->cycle_wait--;

		}

		if (vm->cycle_current > vm->cycle_to_die)
		{
			vm->cycle_current = 0;
			vm_kill_process_if_no_live(vm);
		}

		vm_clean_dead_process(vm);
	}

	vm_destroy(vm);
}