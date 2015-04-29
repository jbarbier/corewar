#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <memory.h>
#include <string.h>
#include "memory.h"
#include "core.h"
#include "vm.h"

#include "display/display.h"

#define MAX_CORES	8
struct s_core_load_informations
{
	int32	address;
	int32	have_address;
	int32	id;	
	char*	file_name;
	t_core*	core;
};

int load_cores(t_vm* vm, int ac, char** av)
{
	struct s_core_load_informations infos[MAX_CORES];
	int32	current_core = 0;
	int32	i;

	for (i = 0; i < MAX_CORES; ++i)
	{
		infos[i].id = 0xcacacaca + i;
		infos[i].address = -1;
		infos[i].have_address = 0;
		infos[i].file_name = NULL;
	}

	for (i = 1; i < ac; ++i)
	{
		if (strcmp("-dump", av[i]) == 0)
			vm->dump_to_cycle = atoi(av[++i]);
		else if (strcmp("-n", av[i]) == 0)
			infos[current_core].id = atoi(av[++i]);
		else if (strcmp("-a", av[i]) == 0)
			infos[current_core].address = atoi(av[++i]), infos[i].have_address = 1;
		else
		{
			t_core* core = core_load_from_file(av[i]);
			if (core)
				infos[current_core++].core = core;
		}			
	}

	
	for (i = 0; i < current_core; ++i)
	{
		if (!infos[i].have_address)
			infos[i].address = (MEM_SIZE / current_core) * i;
		
		while (infos[i].address < 0) infos[i].address += MEM_SIZE;
		vm_add_core(vm, infos[i].core, infos[i].id, infos[i].address % MEM_SIZE);		
	}
	
	return current_core;
}

int main(int ac, char** av)
{	
	t_vm*		vm		= vm_initialize();
	t_process*	process = (t_process*) malloc(sizeof(t_process));
	int32		i;
	t_display*  display;
	int32		update_display = 0;
	int32		was_pressed = 0;
	

	if (load_cores(vm, ac, av) <= 0)
		return -1;

	display = display_initialize(800, 600);


	if (0)
	{
		while (vm->process_count && !display_should_exit(display))
		{
			vm->cycle_current++;
			update_display = 1;
			int process_count = vm->process_count;
			for (i = 0; i < process_count; ++i)
			{
				t_process* process = vm->processes[i];
				if (process->cycle_wait <= 0)
				{
					update_display = 0;
					
					vm_reset_process_io_op(process);					
					vm_execute(vm, process);
					vm_get_opcode(vm, process);
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
//			update_display = 0;
			if (display_update_input(display) || update_display == 0)
				display_step(vm, display);

		}
	}
	else
	{
		int32 execute_one = 0;

		int32 current_keys_state[GLFW_KEY_LAST];
		int32 previous_keys_state[GLFW_KEY_LAST];

		memset(previous_keys_state, 0, GLFW_KEY_LAST * sizeof(int32));
		memset(current_keys_state, 0, GLFW_KEY_LAST * sizeof(int32));
		display_step(vm, display);
		while (vm->process_count && !display_should_exit(display))
		{
			int32 executed = 0;
			int32 print_processes;
			int32 process_count = 0;

			current_keys_state[GLFW_KEY_S] = display_key_pressed(display, GLFW_KEY_S);
			current_keys_state[GLFW_KEY_P] = display_key_pressed(display, GLFW_KEY_P);

			if (!execute_one)
				execute_one = previous_keys_state[GLFW_KEY_S] && !current_keys_state[GLFW_KEY_S];
			print_processes = previous_keys_state[GLFW_KEY_P] && !current_keys_state[GLFW_KEY_P];
			memcpy(previous_keys_state, current_keys_state, sizeof(int32) * GLFW_KEY_LAST);
			
			if (execute_one)
				vm->cycle_current++;
			for (i = 0; i < vm->process_count; ++i)
			{
				t_process* process = vm->processes[i];
				if (print_processes)
					vm_debug_print_process(process);
				if (execute_one)
				{
					if (process->cycle_wait <= 0)
					{
						vm_reset_process_io_op(process);
						vm_execute(vm, process);
						vm_get_opcode(vm, process);
						executed++;
						if (vm->live_count >= NBR_LIVE)
						{
							vm->live_count = 0;
							vm->cycle_to_die -= vm->cycle_delta;
						}
					}
					process->cycle_wait--;
				}
			}
			if (executed)
				execute_one = 0;
			if (vm->cycle_current > vm->cycle_to_die)
			{
				vm->cycle_current = 0;
				vm_kill_process_if_no_live(vm);
			}
			vm_clean_dead_process(vm);
			executed += display_update_input(display);
			if (executed)
				display_step(vm, display);
			else
				glfwPollEvents();
		}
	}



	display_destroy(display);
	vm_destroy(vm);
}
