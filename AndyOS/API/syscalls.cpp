#include "syscalls.h"

int Call(int id, int arg0, int arg1, int arg2, int arg3, int arg4)
{
	int ret;

	asm("int %1\n" 
		"mov %%eax, %0"
		: "=m" (ret) : "N" (SYSCALL_IRQ), "a" (id), "b" (arg0), "c" (arg1), "d" (arg2), "S" (arg3), "D" (arg4));

	return ret;
}