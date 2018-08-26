#include "unistd.h"
#include "sys/syscall.h"
#include "stdarg.h"

char** environ = 0;

int open(const char* filename, int flags)
{
	return syscall2(SYSCALL_OPEN, (int)filename, flags);
}

int close(int fd)
{
	return syscall1(SYSCALL_CLOSE, fd);
}

ssize_t read(int fd, char* buf, size_t size)
{
	return syscall3(SYSCALL_READ, fd, (int)buf, size);
}

ssize_t write(int fd, const char* buf, size_t size)
{
	return syscall3(SYSCALL_WRITE, fd, (int)buf, size);
}

int seek(int fd, long int offset, int origin)
{
	return syscall3(SYSCALL_SEEK, (int)fd, offset, origin);
}

int pipe(int pipefd[2])
{
	return syscall1(SYSCALL_PIPE, (int)pipefd);
}

int dup(int oldfd)
{
	return syscall1(SYSCALL_DUP, oldfd);
}

int dup2(int oldfd, int newfd)
{
	return syscall2(SYSCALL_DUP2, oldfd, newfd);
}

pid_t fork()
{
	return syscall0(SYSCALL_FORK);
}

pid_t getpid()
{
	return syscall0(SYSCALL_GETPID);
}

int execl(char const *path, char const *arg, ...)
{
	int argc = 1;
	va_list	ap;
	va_start(ap, arg);
	while (va_arg(ap, const char*))
		argc++;
	va_end(ap);

	const char* argv[argc + 1];
	va_start(ap, arg);
	argv[0] = arg;
	for (int i = 1; i <= argc; i++)
		argv[i] = va_arg(ap, const char*);
	va_end(ap);

	return execve(path, argv, (const char**)environ);
}

int execlp(char const *file, char const *arg, ...)
{
	return -1;
}

int execv(char const *path, char const *argv[])
{
	return -1;
}

int execvp(char const *file, char const *argv[])
{
	return -1;
}

int execve(char const *path, char const *argv[], char const *envp[])
{
	syscall3(SYSCALL_EXECVE, (size_t)path, (size_t)argv, (size_t)envp);
}

void _exit(int status)
{
    syscall1(SYSCALL_EXIT, status);
}

void sleep(unsigned int seconds)
{
	usleep(seconds * 1000);
}

void usleep(useconds_t usec)
{
	syscall1(SYSCALL_SLEEP, usec);
}