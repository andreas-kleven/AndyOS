#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <signal.h>
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
    return syscall2(SYSCALL_FSTAT, fd, st);
}

pid_t getpid()
{
    return syscall0(SYSCALL_GETPID);
}

int gettimeofday(struct timeval *p, void *z)
{
    return syscall2(SYSCALL_GETTIMEOFDAY, p, z);
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
    return syscall2(SYSCALL_LINK, path1, path2);
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
    return syscall2(SYSCALL_OPEN, name, flags);
}

int pipe(int pipefd[2])
{
    return syscall1(SYSCALL_PIPE, pipefd);
}

ssize_t read(int fd, char *buf, size_t size)
{
    return syscall3(SYSCALL_READ, fd, buf, size);
}

void *sbrk(intptr_t increment)
{
    return syscall1(SYSCALL_SBRK, increment);
}

sig_t signal(int signum, sig_t handler)
{
    return syscall2(SYSCALL_SIGNAL, signum, handler);
}

int stat(const char *file, struct stat *st)
{
    return syscall2(SYSCALL_STAT, file, st);
}

clock_t times(struct tms *buf)
{
    return syscall1(SYSCALL_TIMES, buf);
}

int unlink(char *name)
{
    return syscall1(SYSCALL_UNLINK, name);
}

void usleep(useconds_t usec)
{
    syscall1(SYSCALL_SLEEP, usec);
}

int wait(int *status)
{
    return syscall1(SYSCALL_WAIT, status);
}

ssize_t write(int fd, const char *buf, size_t size)
{
    return syscall3(SYSCALL_WRITE, fd, buf, size);
}

// TODO
void sleep(unsigned int seconds)
{
    usleep(seconds * 1000000);
}
