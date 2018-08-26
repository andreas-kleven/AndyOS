#include "api.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"
#include "syscall.h"

void halt()
{
	syscall(SYSCALL_HALT);
}

void print(const char* msg)
{
	syscall(SYSCALL_PRINT, (int)msg, 0, 0);
}

void color(uint32 color)
{
	syscall(SYSCALL_COLOR, (int)color);
}

void draw(uint32* framebuffer)
{
	syscall(SYSCALL_DRAW, (int)framebuffer);
}

void gettime(int& hour, int& minute, int& second)
{
	syscall(SYSCALL_GETTIME, (int)&hour, (int)&minute, (int)&second);
}

uint32 get_ticks()
{
	return syscall(SYSCALL_GET_TICKS);
}

void exit_thread(int code)
{
	syscall(SYSCALL_EXIT_THREAD, code);
}

bool get_last_key(KEYCODE& code, bool& pressed)
{
	return syscall(SYSCALL_GET_LAST_KEY, (int)&code, (int)&pressed);
}

uint32* alloc(uint32 blocks)
{
	return (uint32*)syscall(SYSCALL_ALLOC, blocks);
}

void free(void* ptr, uint32 blocks)
{
	syscall(SYSCALL_ALLOC, (int)ptr, blocks);
}

void alloc_shared(int proc_id, void*& addr1, void*& addr2, uint32 blocks)
{
	syscall(SYSCALL_ALLOC_SHARED, proc_id, (int)&addr1, (int)&addr2, blocks);
}

int read_file(char*& buffer, char* filename)
{
    return syscall(SYSCALL_READ_FILE, (int)&buffer, (int)filename);
}

int create_process(char* filename)
{
	return syscall(SYSCALL_CREATE_PROCESS, (int)filename);
}

//TODO: remove
void debug_reset()
{
	syscall(SYSCALL_DEBUG_RESET);
}

void debug_print(char* str, ...)
{
    char buffer[256];
	memset(buffer, 0, 256);

	va_list args;
	va_start(args, str);

	vsprintf(buffer, str, args);
    print(buffer);
}