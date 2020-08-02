#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <andyos/api.h>
#include "syscall_list.h"

void halt()
{
	syscall0(SYSCALL_HALT);
}

void print(const char* msg)
{
	syscall3(SYSCALL_PRINT, (int)msg, 0, 0);
}

void color(uint32_t color)
{
	syscall1(SYSCALL_COLOR, (int)color);
}

void draw(uint32_t* framebuffer)
{
	syscall1(SYSCALL_DRAW, (int)framebuffer);
}

void gettime(int& hour, int& minute, int& second)
{
	syscall3(SYSCALL_GETTIME, (int)&hour, (int)&minute, (int)&second);
}

uint32_t get_ticks()
{
	return syscall0(SYSCALL_GET_TICKS);
}

void exit_thread(int code)
{
	syscall1(SYSCALL_EXIT_THREAD, code);
}

/*uint32_t* alloc(uint32_t blocks)
{
	return (uint32_t*)syscall1(SYSCALL_ALLOC, blocks);
}

void free(void* ptr, uint32_t blocks)
{
	syscall2(SYSCALL_ALLOC, (int)ptr, blocks);
}*/

void alloc_shared(int proc_id, void*& addr1, void*& addr2, uint32_t blocks)
{
	syscall4(SYSCALL_ALLOC_SHARED, proc_id, (int)&addr1, (int)&addr2, blocks);
}

int read_file(char*& buffer, const char* filename)
{
    return syscall2(SYSCALL_READ_FILE, (int)&buffer, (int)filename);
}

int create_process(const char* filename)
{
	return syscall1(SYSCALL_CREATE_PROCESS, (int)filename);
}

//TODO: remove
void debug_reset()
{
	syscall0(SYSCALL_DEBUG_RESET);
}

void debug_print(const char* str, ...)
{
    char buffer[256];
	memset(buffer, 0, 256);

	va_list args;
	va_start(args, str);

	vsprintf(buffer, str, args);
    print(buffer);
}