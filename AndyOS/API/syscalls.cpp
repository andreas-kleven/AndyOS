#include "syscalls.h"

int Call(int id, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5)
{
	int ret;

	asm("push %%ebp\n"
		"mov %2, %%ebp\n"
		"int %1\n" 
		"pop %%ebp\n"
		"mov %%eax, %0"
		: "=m" (ret) : "N" (SYSCALL_IRQ), "g" (arg5), "a" (id), "b" (arg0), "c" (arg1), "d" (arg2), "S" (arg3), "D" (arg4));

	return ret;
}