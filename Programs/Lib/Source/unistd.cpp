#include "unistd.h"
#include "syscall.h"

int open(const char* filename, int flags)
{
	return syscall(SYSCALL_OPEN, (int)filename, flags);
}

int close(int fd)
{
	return syscall(SYSCALL_CLOSE, fd);
}

ssize_t read(int fd, char* buf, size_t size)
{
	return syscall(SYSCALL_READ, fd, (int)buf, size);
}

ssize_t write(int fd, const char* buf, size_t size)
{
	return syscall(SYSCALL_WRITE, fd, (int)buf, size);
}

int seek(int fd, long int offset, int origin)
{
	return syscall(SYSCALL_SEEK, (int)fd, offset, origin);
}

int pipe(int pipefd[2])
{
	return syscall(SYSCALL_PIPE, (int)pipefd);
}

pid_t fork()
{
	return syscall(SYSCALL_FORK);
}

pid_t getpid()
{
	return syscall(SYSCALL_GETPID);
}