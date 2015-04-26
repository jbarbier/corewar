#include <stdlib.h>
#include <stdio.h>
#include <io.h>

#include <memory.h>
#include "memory.h"
#include "vm.h"
#include "vm_memory_io.h"

#include "core.h"




t_vm* vm_initialize()
{
	t_vm* vm = (t_vm*) malloc(sizeof(t_vm));

	vm->read_int8 = read_int8_le_vm;
	vm->read_int16 = read_int16_le_vm;
	vm->read_int32 = read_int32_le_vm;
	vm->read_copy = read_copy_vm;

	vm->write_int8 = write_int8_le_vm;
	vm->write_int16 = write_int16_le_vm;
	vm->write_int32 = write_int32_le_vm;
	vm->write_copy = write_copy_vm;

	vm->memory = memory_initialize(MEM_SIZE);
	vm->cycle_current = 0;
	vm->cycle_to_die = CYCLE_TO_DIE;
	vm->cycle_delta = CYCLE_DELTA;
	vm->live_count = NBR_LIVE;
	vm->process_count = 0;
	vm->process_counter = 0;
	vm->processes = (t_process**) malloc(sizeof(t_process*) * VM_MAX_PROCESSES);
	vm->process_count = 0;
		
	vm->processes_pool = (t_process**)malloc(sizeof(t_process*) * VM_MAX_PROCESSES);
	vm->process_pool_count = 0;

	vm->core_count = 0;
	vm->cores = (t_core**)malloc(sizeof(t_core*) * VM_MAX_CORES + 1);
	
	// core[0] is "unknown" core, used when player "live" with a unknow id.
	vm->cores[vm->core_count] = malloc(sizeof(t_core));
	vm->cores[vm->core_count++]->live_count = 0;

	return vm;
}

t_op* vm_get_opcode(t_vm* vm, t_process* process)
{
	t_op* temp = op_tab;
	int8 opcode = vm->read_int8(vm, process->pc);

	while ((temp->code != 0) && (temp->code != opcode))
		temp++;

	process->current_opcode = temp;
	process->cycle_wait = temp->nbr_cycles;
	vm->read_copy(vm, process->pc, 64, process->instruction);
	return temp;
}

void vm_destroy(t_vm* vm)
{
	memory_destroy(vm->memory);
	for (int32 i = 0; i < vm->process_count; ++i)
		if (!vm->processes[i]->free)
			free(vm->processes[i]);
	for (int32 i = 0; i < vm->process_pool_count; ++i)
		free(vm->processes_pool[i]);
	for (int32 i = 0; i < vm->core_count; ++i)
		core_destroy(vm->cores[i]);
	
	free(vm->cores);
	free(vm->processes);
	free(vm->processes_pool);
	
	free(vm);
}


int32 vm_read_value(t_vm* vm, t_process* process, int32* offset, int32 type, int32 mod, int32 dir_as_16)
{
	int32 ret;
	
	if (type == POC_DIR)
	{
		if (dir_as_16)
		{
			ret = read_int16_le(process->instruction + *offset);
			*offset += 2;
		}
		else
		{
			ret = read_int32_le(process->instruction + *offset);
			*offset += 4;
		}
	}
	else if (type == POC_IND)
	{
		ret = read_int16_le(process->instruction + *offset);
		ret = vm->read_int32(vm, process->pc + (ret % mod));
		*offset += 2;
	}
	else
	{		
		ret = *offset;
		ret = process->reg[process->instruction[ret] - 1];
		*offset += 1;
	}
	
	return ret;
}

int32 vm_read_register(t_vm* vm, t_process* process, int32* offset)
{
	int32 ret = process->instruction[*offset] - 1;
	*offset += 1;
	return ret;
}

int32 vm_write_value_mod(t_vm* vm, t_process* process, int32* offset, int32 type, int32 value)
{
	int32 ret;
	
	if (type == POC_IND)
	{
		ret = read_int16_le(process->instruction + *offset);
		vm->write_int32(vm, process->pc + (ret % IDX_MOD), value);
		*offset += 2;
	}
	else
	{
		ret = process->instruction[*offset] - 1;
		process->reg[ret] = value;
		*offset += 1;
	}	
	return ret;
}


int32 vm_write_value(t_vm* vm, t_process* process, int32* offset, int32 type, int32 value)
{
	int32 ret;

	if (type == POC_IND)
	{
		ret = read_int16_le(process->instruction + *offset);
		vm->write_int32(vm, process->pc + ret, value);
		*offset += 2;
	}
	else
	{
		ret = process->instruction[*offset] - 1;
		process->reg[ret] = value;
		*offset += 1;
	}
	return ret;
}


#define TYPE_1(v) ((v >> 6) & 3)
#define TYPE_2(v) ((v >> 4) & 3)
#define TYPE_3(v) ((v >> 2) & 3)
#define TYPE(v, a) ((v >> (8 - (a) * 2)) & 3)

int vm_opcode_check(t_process* process)
{
	for (int i = 0; i < process->current_opcode->nbr_args; ++i)
	{
		int type = TYPE(process->instruction[1], i + 1);
		if ((type & process->current_opcode->type[i]) == 0)
			return VM_ERROR_ENCODING;
	}
	return VM_OK;
}


int vm_execute(t_vm* vm, t_process* process)
{
	int8	encoding;	
	int32   offset = 2;
	int32	arg1, arg2, arg3;	

	vm_debug_print_command(vm, process);
	int32	ret = VM_OK;
	switch (process->current_opcode->code)
	{
	case 1: // live		
		vm->live_count++;
		arg1 = read_int32_le(process->instruction + 1);
		process->cycle_live = vm->cycle_current;
		process->pc += 5;
		break;		
	case 2: // ld
		encoding = read_int8_le(process->instruction + 1);
		arg1 = vm_read_value(vm, process, &offset, TYPE_1(encoding), IDX_MOD, 0);
		arg2 = vm_read_register(vm, process, &offset);

		process->reg[arg2] = arg1;	
		process->carry = (arg1 == 0);
		process->pc += offset;
		break;
	case 3: // st
		encoding = read_int8_le(process->instruction + 1);		
		arg1 = vm_read_value(vm, process, &offset, TYPE_1(encoding), IDX_MOD, 0);
		arg2 = vm_write_value_mod(vm, process, &offset, TYPE_2(encoding), arg1);
		process->pc += offset;
		break;
	case 4: // add
		encoding = read_int8_le(process->instruction + 1);
		arg1 = vm_read_value(vm, process, &offset, TYPE_1(encoding), IDX_MOD, 0);
		arg2 = vm_read_value(vm, process, &offset, TYPE_2(encoding), IDX_MOD, 0);
		arg3 = vm_read_register(vm, process, &offset);
		
		process->reg[arg3] = arg1 + arg2;
		process->carry = (process->reg[arg3] == 0);
		process->pc += offset;
		break;
	case 5: // sub
		encoding = read_int8_le(process->instruction + 1);
		arg1 = vm_read_value(vm, process, &offset, TYPE_1(encoding), IDX_MOD, 0);
		arg2 = vm_read_value(vm, process, &offset, TYPE_2(encoding), IDX_MOD, 0);
		arg3 = vm_read_register(vm, process, &offset);

		process->reg[arg3] = arg1 - arg2;
		process->carry = (process->reg[arg3] == 0);
		process->pc += offset;
		break;
	case 6: // and
		encoding = read_int8_le(process->instruction + 1);
		arg1 = vm_read_value(vm, process, &offset, TYPE_1(encoding), IDX_MOD, 0);
		arg2 = vm_read_value(vm, process, &offset, TYPE_2(encoding), IDX_MOD, 0);
		arg3 = vm_read_register(vm, process, &offset);
		
		process->reg[arg3] = arg1 & arg2;
		process->carry = (process->reg[arg3] == 0);
		process->pc += offset;
		break;
	case 7: // or
		encoding = read_int8_le(process->instruction + 1);
		arg1 = vm_read_value(vm, process, &offset, TYPE_1(encoding), IDX_MOD, 0);
		arg2 = vm_read_value(vm, process, &offset, TYPE_2(encoding), IDX_MOD, 0);
		arg3 = vm_read_register(vm, process, &offset);

		process->reg[arg3] = arg1 | arg2;
		process->carry = (process->reg[arg3] == 0);
		process->pc += offset;
		break;
	case 8: // xor
		encoding = read_int8_le(process->instruction + 1);
		arg1 = vm_read_value(vm, process, &offset, TYPE_1(encoding), IDX_MOD, 0);
		arg2 = vm_read_value(vm, process, &offset, TYPE_2(encoding), IDX_MOD, 0);
		arg3 = vm_read_register(vm, process, &offset);

		process->reg[arg3] = arg1 ^ arg2;
		process->carry = (process->reg[arg3] == 0);
		process->pc += offset;
		break;
	case 9: // zjmp
		arg1 = read_int16_le(process->instruction + 1);
		if (process->carry)
			process->pc = process->pc + arg1 % IDX_MOD;
		else
			process->pc += 3;
		break;

	case 10: // ldi
		encoding = read_int8_le(process->instruction + 1);
		arg1 = read_int16_le(process->instruction + offset), offset += 2;
		arg1 = vm->read_int16(vm, process->pc + arg1 % IDX_MOD);
		arg1 += read_int16_le(process->instruction + offset), offset += 2;
		arg1 = vm->read_int32(vm, process->pc + arg1 % IDX_MOD);
		vm_write_value_mod(vm, process, &offset, (encoding >> 2) & 3, arg1);
		process->carry = arg1 == 0;
		process->pc += offset;
		break;

	case 11: // sti
		encoding = read_int8_le(process->instruction + 1);
		arg1  = vm_read_value(vm, process, &offset, TYPE_1(encoding), IDX_MOD, 1);
		arg2  = vm_read_value(vm, process, &offset, TYPE_2(encoding), IDX_MOD, 1);
		arg2 += vm_read_value(vm, process, &offset, TYPE_3(encoding), IDX_MOD, 1);
		vm->write_int32(vm, arg2, arg1);
		process->pc += offset;
		break;
	
	case 12: // fork
		arg1 = read_int16_le(process->instruction + 1);
		vm_create_process(vm, process, process->pc + arg1 % IDX_MOD);
		process->pc += 3;		
		break;
	
	case 13: // lld
		encoding = read_int8_le(process->instruction + 1);
		arg1 = vm_read_value(vm, process, &offset, TYPE_1(encoding), MEM_SIZE, 0);
		arg2 = vm_read_register(vm, process, &offset);
		process->reg[arg2] = arg1;
		process->carry = (arg1 == 0);
		process->pc += offset;
		break;
	case 14: // lldi
		encoding = read_int8_le(process->instruction + 1);
		arg1 = read_int16_le(process->instruction + offset), offset += 2;
		arg1 = vm->read_int16(vm, process->pc + arg1);
		arg1 += read_int16_le(process->instruction + offset), offset += 2;
		arg1 = vm->read_int32(vm, process->pc + arg1);

		vm_write_value(vm, process, &offset, (encoding >> 2) & 3, arg1);
		process->pc += offset;
		break;
	case 15: // lfork
		arg1 = read_int16_le(process->instruction + 1);
		vm_create_process(vm, process, process->pc + arg1);
		process->pc += 3;
		break;
	case 16: // aff
		encoding = read_int8_le(process->instruction + 1);
		arg1 = vm_read_value(vm, process, &offset, TYPE_1(encoding >> 6), MEM_SIZE, 0);
		uint8 v = (uint8)(arg1 % 0xff);
		_write(0, &v, 1);
		process->pc += offset;
		break;
	default:
		ret = VM_ERROR_OPCODE;
		break;
	}

	if (ret != VM_OK)
		process->pc++;

	process->pc = process->pc % MEM_SIZE;
	return ret;
}
