#ifndef VM_MEMORY_H
#define VM_MEMORY_H

#include "types.h"

struct s_vm;

void	write_int8_le_vm(struct s_vm* vm, int32 offset, int8 value);
void	write_int16_le_vm(struct s_vm* vm, int32 offset, int16 value);
void	write_int32_le_vm(struct s_vm* vm, int32 offset, int32 value);
void	write_copy_vm(struct s_vm* vm, int32 offset, int8* src, int32 size);

int8	read_int8_le_vm(struct s_vm* vm, int32 offset);
int16	read_int16_le_vm(struct s_vm* vm, int32 offset);
int32	read_int32_le_vm(struct s_vm* vm, int32 offset);
void	read_copy_vm(struct s_vm* vm, int32 offset, int32 size, int8* dst);

int8	read_int8_le(int8* ptr);
int16	read_int16_le(int8* ptr);
int32	read_int32_le(int8* ptr);



#endif // !VM_MEMORY_H
