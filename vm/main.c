#include <stdlib.h>
#include <GLFW/glfw3.h>

#include "memory.h"
#include "core.h"
#include "vm.h"

#include "display.h"



int main(int ac, char** av)
{
	t_core*		core	= core_load_from_file(av[1]);
	t_vm*		vm		= vm_initialize();
	t_process*	process = (t_process*) malloc(sizeof(t_process));
	int32		i;
	t_display*  display;
	int32		update_display = 0;

	int32		was_pressed = 0;


	vm_add_core(vm, core, 0xcacacaca, 0);
	memory_flag_reset(vm->memory);
	display = display_initialize();

/*
	while (vm->process_count && !display_should_exit(display))
	{					
		vm->cycle_current++;
		update_display = 1;

		for (i = 0; i < vm->process_count; ++i)
		{
			t_process* process = vm->processes[i];
			if (process->cycle_wait == 0)
			{
				if (update_display)
					memory_flag_reset(vm->memory);
				update_display = 0;
				vm_execute(vm, process);
				vm_get_opcode(vm, process);
				vm_debug_print_command(vm, process);

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
		if (update_display == 0)
			display_step(vm, display);
			
	}
*/
	int32 execute_one = 0;

	while (vm->process_count && !display_should_exit(display))
	{
		int32 is_pressed = display_key_pressed(display, GLFW_KEY_S);
		int32 executed = 0;
		if (!execute_one)
			execute_one = was_pressed && !is_pressed;
		was_pressed = is_pressed;
		
		if (execute_one)
			vm->cycle_current++;
		
		update_display = 1;
		for (i = 0; i < vm->process_count; ++i)
		{
			t_process* process = vm->processes[i];
			if (process->cycle_wait == 0)
			{
/*
				if (update_display)
					memory_flag_reset(vm->memory);
*/
				update_display = 0;
				vm_execute(vm, process);
				vm_get_opcode(vm, process);
				vm_debug_print_command(vm, process);
				executed++;
				if (vm->live_count > NBR_LIVE)
				{
					vm->live_count = 0;
					vm->cycle_to_die -= vm->cycle_delta;
				}				
			}
			else if (execute_one)
				process->cycle_wait--;
		}
		if (executed)
			execute_one = 0;
		
		if (vm->cycle_current > vm->cycle_to_die)
		{
			vm->cycle_current = 0;
			vm_kill_process_if_no_live(vm);
		}

		vm_clean_dead_process(vm);
/*
		if (update_display == 0)
*/
			display_step(vm, display);
	}


	display_destroy(display);
	vm_destroy(vm);
}
