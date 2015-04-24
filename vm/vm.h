#ifndef VM_H
#define VM_H

#include "types.h"
#include "op.h"

struct s_vm;
struct s_op;
struct s_core;

typedef void(*write_int32_t)(struct s_vm* mem, int32 offset, int32 value);
typedef void(*write_int16_t)(struct s_vm* mem, int32 offset, int16 value);
typedef void(*write_int8_t)(struct s_vm* mem, int32 offset, int8 value);
typedef void(*write_copy_t)(struct s_vm* mem, int32 offset, int8* src, int32 size);

typedef int32(*read_int32_t)(struct s_vm* mem, int32 offset);
typedef int16(*read_int16_t)(struct s_vm* mem, int32 offset);
typedef int8(*read_int8_t)(struct s_vm* mem, int32 offset);
typedef void(*read_copy_t)(struct s_vm* mem, int32 offset, int32 size, int8* dst);


#define PROCESS_INSTRUCTION_BUFFER_SIZE 64
#define VM_MAX_PROCESSES				65535

typedef struct s_process
{
	int32			pc;
	int32			reg[REG_NUMBER];
	int32			carry;
	int32			cycle_wait;
	int32			cycle_live;
	struct s_op*	current_opcode;
	int8			instruction[PROCESS_INSTRUCTION_BUFFER_SIZE];
	int32			free;
	int32			internal_id;
	int32			core_id;
} t_process;

struct s_memory;

typedef struct s_vm
{
	struct s_memory*	memory;

	int32		live_count;
	int32		cycle_current;
	int32		cycle_to_die;
	int32		cycle_delta;


	write_int32_t	write_int32;
	write_int16_t	write_int16;
	write_int8_t	write_int8;
	write_copy_t	write_copy;

	read_int8_t		read_int8;
	read_int16_t	read_int16;
	read_int32_t	read_int32;
	read_copy_t		read_copy;

	t_process**		processes;	
	int				process_count;
	
	t_process**		processes_pool;
	int				process_pool_count;
	int				process_counter;
} t_vm;

// in vm_process.c
void		vm_kill_process_if_no_live(t_vm* vm);
void		vm_clean_dead_process(t_vm* vm);
t_process*	vm_create_process(t_vm* vm, t_process* parent, int pc);
t_process*	vm_add_core(t_vm* vm, struct s_core* core, int32 core_id, int32 offset);
// in vm_debug.c
void vm_debug_print_command(t_vm* vm, t_process* process);
//////////////////////////////////////////////////////////////////////////
t_vm*		vm_initialize();
void		vm_destroy(t_vm* vm);
t_op*		vm_get_opcode(t_vm* vm, t_process* process);
void		vm_execute(t_vm* vm, t_process* process);

#endif