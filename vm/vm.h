#ifndef VM_H
#define VM_H

#include "types.h"
#include "op.h"

struct s_vm;
struct s_op;
struct s_core;

typedef void(*write_int32_t)(struct s_vm* mem, struct s_process* process, int32 offset, int32 value);
typedef void(*write_int16_t)(struct s_vm* mem, struct s_process* process, int32 offset, int16 value);
typedef void(*write_int8_t)(struct s_vm* mem, struct s_process* process, int32 offset, int8 value);
typedef void(*write_copy_t)(struct s_vm* mem, int32 offset, int8* src, int32 size);

typedef int32(*read_int32_t)(struct s_vm* mem, struct s_process* process, int32 offset);
typedef int16(*read_int16_t)(struct s_vm* mem, struct s_process* process, int32 offset);
typedef int8(*read_int8_t)(struct s_vm* mem, struct s_process* process, int32 offset);
typedef void(*read_copy_t)(struct s_vm* mem, int32 offset, int32 size, int8* dst);


#define PROCESS_INSTRUCTION_BUFFER_SIZE 16
#define VM_MAX_PROCESSES				65535
#define VM_MAX_CORES					4
#define PROCESS_MAX_READ_OP				32
#define PROCESS_MAX_WRITE_OP			32

#define VM_ERROR_ENCODING	-1
#define VM_ERROR_OPCODE		-2
#define VM_ERROR_REGISTER	-3

#define VM_OK			1

typedef struct s_process
{
	int32			pc;
	int32			next_pc;
	int32			reg[REG_NUMBER];
	int32			carry;
	int32			cycle_wait;
	int32			cycle_live;
	struct s_op*	current_opcode;
	int8			instruction[PROCESS_INSTRUCTION_BUFFER_SIZE];
	int32			free;
	int32			internal_id;
	int32			core_id;
	int32			memory_read_op[PROCESS_MAX_READ_OP];
	int32			memory_read_op_count;
	int32			memory_write_op[PROCESS_MAX_WRITE_OP];
	int32			memory_write_op_count;
	
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
	int32			process_count;
	
	t_process**		processes_pool;
	int32			process_pool_count;
	int32			process_counter;

	struct s_core**	cores;
	int32			core_count;

	int32			dump_to_cycle;
} t_vm;

//////////////////////////////////////////////////////////////////////////
// in vm_process.c
void		vm_kill_process_if_no_live(t_vm* vm);
void		vm_clean_dead_process(t_vm* vm);
t_process*	vm_create_process(t_vm* vm, t_process* parent, int pc);
t_process*	vm_add_core(t_vm* vm, struct s_core* core, int32 core_id, int32 offset);
void		vm_reset_process_io_op(t_process* process);

//////////////////////////////////////////////////////////////////////////
// in vm_debug.c
void		vm_debug_print_command(t_vm* vm, t_process* process);
void		vm_debug_print_process(t_process* process);
//////////////////////////////////////////////////////////////////////////
// in vm.c
t_vm*		vm_initialize();
void		vm_destroy(t_vm* vm);
t_op*		vm_get_opcode(t_vm* vm, t_process* process);
int			vm_execute(t_vm* vm, t_process* process);

#endif