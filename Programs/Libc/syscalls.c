#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include "syscall_list.h"

int set_err(int ret)
{
    if (ret < 0)
    {
        errno = ret;
        return -1;
    }

    return ret;
}

//

int chdir(const char *path)
{
    return 0;
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
    return 0;
}

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
    return 0;
}

int sigsuspend(const sigset_t *mask)
{
    return 0;
}

mode_t umask(mode_t mask)
{
    return 0;
}

int lstat(const char *path, struct stat *buf)
{
    return 0;
}

pid_t wait3(int *wstatus, int options, struct rusage *rusage)
{
    return 0;
}

int setpgid(pid_t pid, pid_t pgid)
{
    return 0;
}

pid_t getpgid(pid_t pid)
{
    return 0;
}

pid_t getppid()
{
    return 0;
}

uid_t getuid()
{
    return 0;
}

uid_t getgid()
{
    return 0;
}

uid_t geteuid()
{
    return 0;
}

uid_t getegid()
{
    return 0;
}

int getgroups(int size, gid_t list[])
{
    return 0;
}

pid_t getpgrp()
{
    return 0;
}

pid_t tcgetpgrp(int fd)
{
    return 0;
}

int tcsetpgrp(int fd, pid_t pgrp)
{
    return 0;
}

pid_t vfork()
{
    pid_t pid;

    pid = fork();

    if (!pid)
    {
        return 0;
    }
    else
    {
        //TODO
        while (true)
            sleep(1000);
        
        //if (waitpid(pid, NULL, 0) < 0)
            return pid;
    }
}

////

int *__errno()
{
    return &_REENT->_errno;
}

void _exit(int status)
{
    syscall1(SYSCALL_EXIT, status);
}

int _close(int fd)
{
    return set_err(syscall1(SYSCALL_CLOSE, fd));
}

int dup(int oldfd)
{
    return set_err(syscall1(SYSCALL_DUP, oldfd));
}

int dup2(int oldfd, int newfd)
{
    return set_err(syscall2(SYSCALL_DUP2, oldfd, newfd));
}

int _execve(const char *path, char *const argv[], char *const envp[])
{
    return set_err(syscall3(SYSCALL_EXECVE, (size_t)path, (size_t)argv, (size_t)envp));
}

int _fcntl(int fd, int cmd, ...)
{
    int arg = 0;
    if ((cmd == F_DUPFD) || (cmd == F_SETFD) || (cmd == F_SETFL))
    {
        va_list args;
        va_start(args, cmd);
        arg = va_arg(args, int);
        va_end(args);
    }

    return set_err(syscall3(SYSCALL_FCNTL, fd, cmd, arg));
}

pid_t _fork()
{
    return set_err(syscall0(SYSCALL_FORK));
}

int _fstat(int fd, struct stat *st)
{
    return set_err(syscall2(SYSCALL_FSTAT, fd, st));
}

pid_t _getpid()
{
    return set_err(syscall0(SYSCALL_GETPID));
}

/*int getdents(unsigned int fd, struct dirent *dirp, unsigned int count)
{
    return syscall3(SYSCALL_GETDENTS, fd, dirp, count);
}*/

int getdents(int fd, void *dp, int count)
{
    return set_err(syscall3(SYSCALL_GETDENTS, fd, dp, count));
}

int _gettimeofday(struct timeval *p, void *z)
{
    return set_err(syscall2(SYSCALL_GETTIMEOFDAY, p, z));
}

int _isatty(int fd)
{
    return 0;
}

int _kill(int pid, int sig)
{
    return set_err(syscall2(SYSCALL_KILL, pid, sig));
}

int _link(char *path1, char *path2)
{
    return set_err(syscall2(SYSCALL_LINK, path1, path2));
}

off_t _lseek(int fd, off_t offset, int whence)
{
    return set_err(syscall3(SYSCALL_LSEEK, fd, offset, whence));
}

// TODO
//int open(const char *pathname, int flags);
//int open(const char *pathname, int flags, mode_t mode);
int open(const char *name, int flags, ...)
{
    return set_err(syscall2(SYSCALL_OPEN, name, flags));
}

int pipe(int pipefd[2])
{
    return set_err(syscall1(SYSCALL_PIPE, pipefd));
}

ssize_t _read(int fd, char *buf, size_t size)
{
    return set_err(syscall3(SYSCALL_READ, fd, buf, size));
}

caddr_t _sbrk(intptr_t increment)
{
    return (caddr_t)set_err(syscall1(SYSCALL_SBRK, increment));
}

sig_t signal(int signum, sig_t handler)
{
    return (sig_t)set_err(syscall2(SYSCALL_SIGNAL, signum, handler));
}

int _stat(const char *file, struct stat *st)
{
    return set_err(syscall2(SYSCALL_STAT, file, st));
}

clock_t _times(struct tms *buf)
{
    return set_err(syscall1(SYSCALL_TIMES, buf));
}

int _unlink(char *name)
{
    return set_err(syscall1(SYSCALL_UNLINK, name));
}

int usleep(useconds_t usec)
{
    return set_err(syscall1(SYSCALL_SLEEP, usec));
}

int _wait(int *status)
{
    return set_err(syscall1(SYSCALL_WAIT, status));
}

ssize_t _write(int fd, const char *buf, size_t size)
{
    return (ssize_t)set_err(syscall3(SYSCALL_WRITE, fd, buf, size));
}

//

int socket(int domain, int type, int protocol)
{
    return set_err(syscall3(SYSCALL_SOCKET, domain, type, protocol));
}

int accept(int fd, struct sockaddr *addr, socklen_t addrlen, int flags)
{
    return set_err(syscall4(SYSCALL_ACCEPT, fd, addr, addrlen, flags));
}

int bind(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
    return set_err(syscall3(SYSCALL_BIND, fd, addr, addrlen));
}

int connect(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
    return set_err(syscall3(SYSCALL_CONNECT, fd, addr, addrlen));
}

int listen(int fd, int backlog)
{
    return set_err(syscall2(SYSCALL_LISTEN, fd, backlog));
}

int recv(int fd, void *buf, size_t len, int flags)
{
    return set_err(syscall4(SYSCALL_RECV, fd, buf, len, flags));
}

int recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t addrlen)
{
    return set_err(syscall6(SYSCALL_RECVFROM, fd, buf, len, flags, src_addr, addrlen));
}

int recvmsg(int fd, struct msghdr *msg, int flags)
{
    return set_err(syscall3(SYSCALL_RECVMSG, fd, msg, flags));
}

int send(int fd, const void *buf, size_t len, int flags)
{
    return set_err(syscall4(SYSCALL_SEND, fd, buf, len, flags));
}

int sendmsg(int fd, const struct msghdr *msg, int flags)
{
    return set_err(syscall3(SYSCALL_SENDMSG, fd, msg, flags));
}

int sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
    return set_err(syscall6(SYSCALL_SENDTO, fd, buf, len, flags, dest_addr, addrlen));
}

int shutdown(int fd, int how)
{
    return set_err(syscall2(SYSCALL_SHUTDOWN, fd, how));
}

unsigned int sleep(unsigned int seconds)
{
    return usleep(seconds * 1000000);
}
