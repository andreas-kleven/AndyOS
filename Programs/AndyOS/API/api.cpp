#include "api.h"
#include "stdarg.h"
#include "string.h"
#include "stdio.h"

int open(const char* filename, int flags)
{
	return Call(SYSCALL_OPEN, (int)filename, flags);
}

int close(int fd)
{
	return Call(SYSCALL_CLOSE, fd);
}

size_t read(int fd, char* buf, size_t size)
{
	return Call(SYSCALL_READ, fd, (int)buf, size);
}

size_t write(int fd, const char* buf, size_t size)
{
	return Call(SYSCALL_WRITE, fd, (int)buf, size);
}

int seek(int fd, long int offset, int origin)
{
	return Call(SYSCALL_SEEK, (int)fd, offset, origin);
}

int pipe(int pipefd[2])
{
	return Call(SYSCALL_PIPE, (int)pipefd);
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

void draw(uint32* framebuffer)
{
	Call(SYSCALL_DRAW, (int)framebuffer);
}

void gettime(int& hour, int& minute, int& second)
{
	Call(SYSCALL_GETTIME, (int)&hour, (int)&minute, (int)&second);
}

uint32 get_ticks()
{
	return Call(SYSCALL_GET_TICKS);
}

void exit(int code)
{
	Call(SYSCALL_EXIT, code);
}

void exit_thread(int code)
{
	Call(SYSCALL_EXIT_THREAD, code);
}

void sleep(uint32 ticks)
{
	Call(SYSCALL_SLEEP, ticks);
}

bool get_last_key(KEYCODE& code, bool& pressed)
{
	return Call(SYSCALL_GET_LAST_KEY, (int)&code, (int)&pressed);
}

uint32* alloc(uint32 blocks)
{
	return (uint32*)Call(SYSCALL_ALLOC, blocks);
}

void free(void* ptr, uint32 blocks)
{
	Call(SYSCALL_ALLOC, (int)ptr, blocks);
}

void alloc_shared(int proc_id, void*& addr1, void*& addr2, uint32 blocks)
{
	Call(SYSCALL_ALLOC_SHARED, proc_id, (int)&addr1, (int)&addr2, blocks);
}

int read_file(char*& buffer, char* filename)
{
    return Call(SYSCALL_READ_FILE, (int)&buffer, (int)filename);
}

int create_process(char* filename)
{
	return Call(SYSCALL_CREATE_PROCESS, (int)filename);
}

//TODO: remove
void debug_reset()
{
	Call(SYSCALL_DEBUG_RESET);
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