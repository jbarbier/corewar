#include <stdio.h>
#include "vm.h"
#include "vm_memory_io.h"
#include "memory.h"
#include "op.h"

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

char vm_debug_get_param_desc(int32 type)
{
	if (type == POC_DIR)
		return '%';
	else if (type == POC_IND)
		return ' ';
	return 'r';
}
void vm_debug_print_command(t_vm* vm, t_process* process)
{
	int offset_encoding = 6;
	int offset = 1;
	int type;
	int8 encoding;

	if (process->current_opcode->code)
	{
		printf("%p(%d) %s\t", vm->memory->data + process->pc, process->internal_id, process->current_opcode->mnemonique);
		switch (process->current_opcode->code)
		{
		case 1:
			printf("%d", vm_debug_read_value(process, &offset, POC_DIR));
			break;
		case 9:
		case 12:
		case 15:
			printf("%d", vm_debug_read_value(process, &offset, POC_IND));
			break;
		default:
			offset = 2;
			encoding = read_int8_le(process->instruction + 1);
			for (int i = 0; i < process->current_opcode->nbr_args; ++i)
			{
				type = (encoding >> offset_encoding) & 3;
				printf("%c%d", vm_debug_get_param_desc(type), vm_debug_read_value(process, &offset, type));
				offset_encoding -= 2;
				if (i < (process->current_opcode->nbr_args - 1))
					printf(",");
			}
			break;
		}

		printf("\n");
	}
}
