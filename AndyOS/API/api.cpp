#include "api.h"
#include "syscall_list.h"
#include "stdarg.h"

#define CALL0(id)	_asm mov eax, id \
					_asm int SYSCALL_IRQ

#define CALL1(id, arg0)	_asm mov ebx, arg0 \
						CALL0(id)

#define CALL2(id, arg0, arg1)	_asm mov ecx, arg1 \
								CALL1(id, arg0)

#define CALL3(id, arg0, arg1, arg2)	_asm mov edx, arg2 \
									CALL2(id, arg0, arg1)

#define RET { uint32* _retval; \
			  _asm { mov [_retval], eax } \
			  return _retval; }

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
	RET;
}

void free(void* _ptr, uint32 blocks)
{
	CALL2(SYSCALL_ALLOC, _ptr, blocks);
}