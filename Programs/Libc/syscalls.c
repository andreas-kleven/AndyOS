#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>
#include "syscall_list.h"

char **environ = 0;

int *__errno()
{
    return &_REENT->_errno;
}

void _exit(int status)
{
    syscall1(SYSCALL_EXIT, status);
}

int close(int fd)
{
    return syscall1(SYSCALL_CLOSE, fd);
}

int dup(int oldfd)
{
    return syscall1(SYSCALL_DUP, oldfd);
}

int dup2(int oldfd, int newfd)
{
    return syscall2(SYSCALL_DUP2, oldfd, newfd);
}

int execve(const char *path, char const *argv[], char const *envp[])
{
    return syscall3(SYSCALL_EXECVE, (size_t)path, (size_t)argv, (size_t)envp);
}

pid_t fork()
{
    return syscall0(SYSCALL_FORK);
}

int fstat(int fd, struct stat *st)
{
    return syscall2(SYSCALL_FSTAT, fd, (int)st);
}

pid_t getpid()
{
    return syscall0(SYSCALL_GETPID);
}

int gettimeofday(struct timeval *p, void *z)
{
    return syscall2(SYSCALL_GETTIMEOFDAY, (int)p, (int)z);
}

int isatty(int fd)
{
    return 0;
}

int kill(int pid, int sig)
{
    return syscall2(SYSCALL_KILL, pid, sig);
}

int link(char *path1, char *path2)
{
    return syscall2(SYSCALL_LINK, (int)path1, (int)path2);
}

off_t lseek(int fd, off_t offset, int whence)
{
    return syscall3(SYSCALL_LSEEK, fd, offset, whence);
}

// TODO
//int open(const char *pathname, int flags);
//int open(const char *pathname, int flags, mode_t mode);
int open(const char *name, int flags, ...)
{
    return syscall2(SYSCALL_OPEN, (int)name, flags);
}

int pipe(int pipefd[2])
{
    return syscall1(SYSCALL_PIPE, (int)pipefd);
}

ssize_t read(int fd, char *buf, size_t size)
{
    return syscall3(SYSCALL_READ, fd, (int)buf, size);
}

void *sbrk(intptr_t increment)
{
    return syscall1(SYSCALL_SBRK, increment);
}

int stat(const char *file, struct stat *st)
{
    return syscall2(SYSCALL_STAT, (int)file, (int)st);
}

clock_t times(struct tms *buf)
{
    return syscall1(SYSCALL_TIMES, (int)buf);
}

int unlink(char *name)
{
    return syscall1(SYSCALL_UNLINK, (int)name);
}

void usleep(useconds_t usec)
{
    syscall1(SYSCALL_SLEEP, usec);
}

int wait(int *status)
{
    return syscall1(SYSCALL_WAIT, (int)status);
}

ssize_t write(int fd, const char *buf, size_t size)
{
    return syscall3(SYSCALL_WRITE, fd, (int)buf, size);
}

// TODO
void sleep(unsigned int seconds)
{
    usleep(seconds * 1000);
}
