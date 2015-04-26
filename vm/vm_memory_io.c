#include <memory.h>
#include "op.h"
#include "vm.h"
#include "memory.h"

void write_int8_le_vm(struct s_vm* vm, int32 offset, int8 value)
{
	while (offset < 0) offset += MEM_SIZE;
	int8* ptr = vm->memory->data + offset % MEM_SIZE;
	*ptr = (int8)(value & 0xff);
}


void write_int16_le_vm(struct s_vm* vm, int32 offset, int16 value)
{	
	write_int8_le_vm(vm, offset, (int8)((value >> 8) & 0xff));
	write_int8_le_vm(vm, offset + 1, (int8)(value & 0xff));
}

void write_int32_le_vm(struct s_vm* vm, int32 offset, int32 value)
{	
	write_int8_le_vm(vm, offset + 0, (int8)((value >> 24) & 0xff));
	write_int8_le_vm(vm, offset + 1, (int8)((value >> 16) & 0xff));
	write_int8_le_vm(vm, offset + 2, (int8)((value >> 8) & 0xff));
	write_int8_le_vm(vm, offset + 3, (int8)((value >> 0) & 0xff));
}

void write_copy_vm(struct s_vm* vm, int32 offset, int8* src, int32 size)
{
	while (offset < 0) offset += MEM_SIZE;
	offset %= MEM_SIZE;

	if ( (offset + size) > MEM_SIZE)
	{
		int count = (MEM_SIZE - offset);
		memcpy(vm->memory->data + offset, src, count);
		memcpy(vm->memory->data, src + count, size - count);
	}
	else
		memcpy(vm->memory->data + offset, src, size);
}


//////////////////////////////////////////////////////////////////////////

int8 read_int8_le_vm(struct s_vm* vm, int32 offset)
{
	while (offset < 0) offset += MEM_SIZE;
	return *(vm->memory->data + offset % MEM_SIZE);
}

int16 read_int16_le_vm(struct s_vm* vm, int32 offset)
{
	int16 dword[2];
	dword[0] = (int16)(read_int8_le_vm(vm, offset + 0)) & 0xff;
	dword[1] = (int16)(read_int8_le_vm(vm, offset + 1)) & 0xff;
	return dword[0] << 8 | dword[1];
}

int32 read_int32_le_vm(struct s_vm* vm, int32 offset)
{
	int32 dword[4];
	dword[0] = (int32)(read_int8_le_vm(vm, offset + 0)) & 0xff;
	dword[1] = (int32)(read_int8_le_vm(vm, offset + 1)) & 0xff;
	dword[2] = (int32)(read_int8_le_vm(vm, offset + 2)) & 0xff;
	dword[3] = (int32)(read_int8_le_vm(vm, offset + 3)) & 0xff;

	return dword[0] << 24 | dword[1] << 16 | dword[2] << 8 | dword[3] << 0;
}

void read_copy_vm(struct s_vm* vm, int32 offset, int32 size, int8* dst)
{
	while (offset < 0) offset += MEM_SIZE;
	offset %= MEM_SIZE;

	if (offset + size > MEM_SIZE)
	{
		int count = (MEM_SIZE - offset);
		memcpy(dst, vm->memory->data + offset, count);
		memcpy(dst + count, vm->memory->data, size - count);
	}
	else
		memcpy(dst, vm->memory->data + offset, size);
}

//////////////////////////////////////////////////////////////////////////

int8	read_int8_le(int8* data)
{
	return *(data);
}

int16	read_int16_le(int8* ptr)
{
	int16 msb = ((int16)*(ptr++)) & 0xff;
	int16 lsb = ((int16)*(ptr)) & 0xff;

	return (msb << 8) | lsb;
}

int32	read_int32_le(int8* ptr)
{
	int32 dword[4] = { ((int32)*(ptr)) & 0xff,
		((int32)*(ptr + 1)) & 0xff,
		((int32)*(ptr + 2)) & 0xff,
		((int32)*(ptr + 3)) & 0xff };

	return dword[0] << 24 | dword[1] << 16 | dword[2] << 8 | dword[3] << 0;
}

