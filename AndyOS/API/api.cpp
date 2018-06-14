#include "api.h"
#include "syscall_list.h"
#include "stdarg.h"

#define CALL0(id)	asm volatile("mov (%0), %%eax" :: "d" (int(id))); \
					asm volatile("int %0" :: "N" (SYSCALL_IRQ));

#define CALL1(id, arg0)	asm volatile("mov (%0), %%ebx" :: "d" ((int(arg0)))); \
						CALL0(id);

#define CALL2(id, arg0, arg1)	asm volatile("mov (%0), %%ecx" :: "d" ((int(arg1)))); \
								CALL1(id, arg0);

#define CALL3(id, arg0, arg1, arg2)		asm volatile("mov (%0), %%edx" :: "d" ((int(arg2)))); \
										CALL2(id, arg1, arg2);

void halt()
{
	CALL0(SYSCALL_HALT);
}

void print(const char* msg)
{
	CALL1(SYSCALL_PRINT, msg);
}

void print(uint32 color)
{
	CALL1(SYSCALL_COLOR, color);
}

void gettime(int& hour, int& minute, int& second)
{
	CALL3(SYSCALL_GETTIME, hour, minute, second);
}

void draw(uint32* framebuffer)
{
	CALL1(SYSCALL_DRAW, framebuffer);
}

void exit(int code)
{
	CALL1(SYSCALL_EXIT, code);
}

void sleep(uint32 ticks)
{
	CALL1(SYSCALL_SLEEP, ticks);
}

uint32 ticks()
{
	CALL0(SYSCALL_TICKS);
}

void get_mouse_pos(int& x, int& y)
{
	CALL2(SYSCALL_GET_MOUSE_POS, x, y);
}

void get_mouse_buttons(bool& left, bool& right, bool& middle)
{
	CALL3(SYSCALL_GET_MOUSE_BUTTONS, left, right, middle);
}

uint32* alloc(uint32 blocks)
{
	CALL1(SYSCALL_ALLOC, blocks);
	
	uint32* ret;
	asm volatile("mov %%eax, %0" : "=m" (ret));
	return ret;
}

void free(void* _ptr, uint32 blocks)
{
	CALL2(SYSCALL_ALLOC, _ptr, blocks);
}