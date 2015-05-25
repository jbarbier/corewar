#include <stdio.h>
#include <stdarg.h>


#include "../common/utils.h"

#include "vm.h"
#include "vm_memory_io.h"
#include "memory.h"

int32 vm_debug_read_value(t_process* process, int32* offset, int32 type)
{
	int32 ret;
	if (type == POC_DIR)
	{
		ret = read_int32_le(process->instruction + *offset);
		*offset += 4;
	}
	else if (type == POC_IND)
	{
		ret = read_int16_le(process->instruction + *offset);
		*offset += 2;
	}
	else
	{
		ret = *offset;
		ret = process->instruction[ret];
		*offset += 1;
	}
	return ret;
}

char* vm_debug_get_param_desc(int32 type)
{
	if (type == POC_DIR)
		return "%";
	else if (type == POC_IND)
		return "";
	return "r";
}

void vm_printf(t_vm* vm, int32 max_size, char* format, ...)
{
	if (vm->print_callback)
	{
		va_list args;
		char	buffer[256];
		int32	size = 0;

		va_start(args, format);
		size = vsprintf(buffer, format, args);
		va_end(args);
		size = max_size - size;
		if (size > 0)
		{
			sprintf(buffer, "%s%*s", buffer, size, " ");
		}

		vm->print_callback(vm, buffer, vm->print_callback_userdata);
	}


}

void vm_debug_print_process(t_vm* vm, t_process* process)
{
	int32 i;

	vm_printf(vm, 0, "-------------------------------------------------------------------------------\n");
	vm_printf(vm, 0, "process %d\n", process->internal_id);
	vm_printf(vm, 0, "pc=0x%0.8x (%d)\n", process->pc, process->pc);
	vm_printf(vm, 0, "carry=%d\n", process->carry);
	for (i = 0; i < REG_NUMBER; ++i)
	{
		vm_printf(vm, 4, "r%0.2d", i + 1, process->reg[i], process->reg[i]);
		vm_printf(vm, 0, "= 0x%0.8x", process->reg[i]);
		vm_printf(vm, 0, "(%d)\n", process->reg[i]);
	}
	vm_printf(vm, 0, "-------------------------------------------------------------------------------\n");
}

void vm_debug_print_command(t_vm* vm, t_process* process)
{
	int32 	offset_encoding = 6;
	int32 	offset = 1;
	int32 	type;
	int32	i;
	int8 	encoding;
	int32 	special = 0;

	vm_printf(vm, 15, "%p(%d)", vm->memory->data + process->pc, process->internal_id);
	if (process->current_opcode->code)
	{
		vm_printf(vm, 8, "%s", process->current_opcode->mnemonique);
		switch (process->current_opcode->code)
		{
		case 1:
			vm_printf(vm, 0, "%d", vm_debug_read_value(process, &offset, POC_DIR));
			break;
		case 9:
		case 12:
		case 15:
			vm_printf(vm, 0, "%d", vm_debug_read_value(process, &offset, POC_IND));
			break;
		case 10:
		case 11:
		case 14:
			special = 1;
			// special case : sti, ldi, lldi direct values 16 bits.
		default:
			offset = 2;
			encoding = read_int8_le(process->instruction + 1);
			for (i = 0; i < process->current_opcode->nbr_args; ++i)
			{
				type = (encoding >> offset_encoding) & 3;
				if (special && type == POC_DIR) type = POC_IND;
				vm_printf(vm, 0, "%s%d", vm_debug_get_param_desc(type), vm_debug_read_value(process, &offset, type));
				offset_encoding -= 2;
				if (i < (process->current_opcode->nbr_args - 1))
					vm_printf(vm, 0, ",");
			}
			break;
		}

		vm_printf(vm, 0, "\n");
	}
	else
		vm_printf(vm, 0, "nop\n");
}
