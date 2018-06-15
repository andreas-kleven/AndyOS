#include "api.h"
#include "syscall_list.h"
#include "stdarg.h"

int Call(int id, int arg0 = 0, int arg1 = 0, int arg2 = 0)
{
	int ret;

	asm("int %1\n" 
		"mov %%eax, %0"
		: "=m" (ret) : "N" (SYSCALL_IRQ), "a" (id), "b" (arg0), "c" (arg1), "d" (arg2));

	return ret;
}

void halt()
{
	Call(SYSCALL_HALT);
}

void print(const char* msg)
{
	Call(SYSCALL_PRINT, (int)msg, 0, 0);
}

void color(uint32 color)
{
	Call(SYSCALL_COLOR, (int)color);
}

void gettime(int& hour, int& minute, int& second)
{
	Call(SYSCALL_GETTIME, (int)&hour, (int)&minute, (int)&second);
}

void draw(uint32* framebuffer)
{
	Call(SYSCALL_DRAW, (int)framebuffer);
}

void exit(int code)
{
	Call(SYSCALL_EXIT, code);
}

void sleep(uint32 ticks)
{
	Call(SYSCALL_SLEEP, ticks);
}

uint32 ticks()
{
	return Call(SYSCALL_TICKS);
}

void get_mouse_pos(int& x, int& y)
{
	Call(SYSCALL_GET_MOUSE_POS, (int)&x, (int)&y);
}

void get_mouse_buttons(bool& left, bool& right, bool& middle)
{
	Call(SYSCALL_GET_MOUSE_BUTTONS, (int)&left, (int)&right, (int)&middle);
}

uint32* alloc(uint32 blocks)
{
	return (uint32*)Call(SYSCALL_ALLOC, blocks);
}

void free(void* ptr, uint32 blocks)
{
	Call(SYSCALL_ALLOC, (int)ptr, blocks);
}