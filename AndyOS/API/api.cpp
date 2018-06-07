#include "api.h"
#include "syscalls.h"
#include "stdarg.h"

#define CALL0(id)	_asm mov eax, id \
					_asm int SYSCALL_IRQ

#define CALL1(id, arg0)	_asm mov ebx, arg0 \
						CALL0(id)

#define CALL2(id, arg0, arg1)	_asm mov ecx, arg1 \
								CALL1(id, arg0)

#define CALL3(id, arg0, arg1, arg2)	_asm mov edx, arg2 \
									CALL2(id, arg0, arg1)

void halt()
{
	CALL0(SYSCALL_HALT);
}

void print(const char* msg)
{
	CALL1(SYSCALL_PRINT, msg);
}

void gettime(int& hour, int& minute, int& second)
{
	CALL3(SYSCALL_GETTIME, hour, minute, second);
}