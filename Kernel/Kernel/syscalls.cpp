#include <Arch/syscalls.h>
#include <Drivers/keyboard.h>
#include <Drivers/mouse.h>
#include <Drivers/rtc.h>
#include <Drivers/vtty.h>
#include <FS/vfs.h>
#include <Kernel/timer.h>
#include <Net/socket.h>
#include <Process/dispatcher.h>
#include <Process/process.h>
#include <Process/scheduler.h>
#include <debug.h>
#include <errno.h>
#include <hal.h>
#include <irq.h>
#include <memory.h>
#include <string.h>
#include <syscall_list.h>
#include <syscalls.h>
#include <time.h>
#include <utsname.h>
#include <video.h>

namespace Syscalls {
SYSCALL_HANDLER syscalls[MAX_SYSCALLS];

Filetable *CurrentFiletable()
{
    return Dispatcher::CurrentProcess()->filetable;
}

int sys_open(const char *filename, int flags, mode_t mode)
{
    return VFS::Open(Dispatcher::CurrentProcess(), filename, flags, mode);
}

int sys_close(int fd)
{
    return VFS::Close(CurrentFiletable(), fd);
}

size_t sys_read(int fd, char *buf, size_t size)
{
    return VFS::Read(CurrentFiletable(), fd, buf, size);
}

size_t sys_write(int fd, const char *buf, size_t size)
{
    return VFS::Write(CurrentFiletable(), fd, buf, size);
}

off_t sys_lseek(int fd, off_t offset, int whence)
{
    return VFS::Seek(CurrentFiletable(), fd, offset, whence);
}

int sys_pipe(int pipefd[2])
{
    return VFS::CreatePipes(CurrentFiletable(), pipefd, 0);
}

int sys_dup(int oldfd)
{
    return VFS::DuplicateFile(CurrentFiletable(), oldfd);
}

int sys_dup2(int oldfd, int newfd)
{
    return VFS::DuplicateFile(CurrentFiletable(), oldfd, newfd);
}

int sys_fcntl(int fd, int cmd, void *arg)
{
    return VFS::Fcntl(CurrentFiletable(), fd, cmd, arg);
}

int sys_gettimeofday(struct timeval *tv, void *tz)
{
    struct tm t = RTC::Time();
    time_t time = mktime(&t);
    tv->tv_sec = time;
    tv->tv_usec = 0;
    return 0;
}

int sys_getdents(int fd, dirent *dirp, unsigned int count)
{
    return VFS::Getdents(CurrentFiletable(), fd, dirp, count);
}

pid_t sys_fork()
{
    PROCESS *newproc = ProcessManager::Fork(Dispatcher::CurrentProcess());
    return newproc ? newproc->id : -1;
}

int sys_execve(char const *path, char const *argv[], char const *envp[])
{
    PROCESS *proc = Dispatcher::CurrentProcess();
    return ProcessManager::Exec(proc, path, argv, envp);
}

void *sys_sbrk(intptr_t increment)
{
    PROCESS *proc = Dispatcher::CurrentProcess();
    void *ret = ProcessManager::AdjustHeap(proc, increment);

    if (ret)
        return ret;

    return (void *)-1;
}

int sys_stat(const char *filename, stat *st)
{
    return VFS::Stat(Dispatcher::CurrentProcess(), filename, st);
}

int sys_fstat(int fd, stat *st)
{
    return VFS::Fstat(CurrentFiletable(), fd, st);
}

int sys_kill(pid_t pid, int signo)
{
    PROCESS *current = Dispatcher::CurrentProcess();

    if (pid == -1) {
        panic("Kill", "pid = -1");
        return -1;
    } else if (pid < 0) {
        return ProcessManager::HandleSignal(current->sid, -pid, signo);
    } else if (pid == 0) {
        return ProcessManager::HandleSignal(current->sid, current->gid, signo);
    } else {
        PROCESS *proc = ProcessManager::GetProcess(pid);
        return ProcessManager::HandleSignal(proc, signo);
    }
}

int sys_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
    kprintf("sigaction %d %p %p\n", signum, act, oldact);

    if (act)
        kprintf("act %p %p %p\n", act->sa_handler, act->sa_mask, act->sa_flags);

    sig_t handler = act ? act->sa_handler : 0;

    PROCESS *proc = Dispatcher::CurrentProcess();
    sig_t old = ProcessManager::SetSignalHandler(proc, signum, handler);

    if (oldact) {
        oldact->sa_handler = old < 0 ? 0 : old;
        oldact->sa_mask = 0;
        oldact->sa_flags = 0;
    }

    return (int)old < 0 ? -1 : 0;
}

int sys_sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
    kprintf("sigprocmask %p %p %p\n", how, set, oset);

    if (set)
        kprintf("set %p\n", *set);

    return 0;
}

int sys_sigreturn()
{
    return ProcessManager::FinishSignal(Dispatcher::CurrentThread());
}

int sys_sigsuspend(const sigset_t *mask)
{
    kprintf("sigsuspend %d\n", mask);

    if (mask)
        kprintf("mask %p\n", *mask);

    return -EINTR;
}

int sys_waitpid(pid_t pid, int *status, int options)
{
    return Dispatcher::Waitpid(pid, status, options);
}

int sys_chdir(const char *path)
{
    PROCESS *proc = Dispatcher::CurrentProcess();
    return ProcessManager::Chdir(proc, path);
}

int sys_fchdir(int fd)
{
    PROCESS *proc = Dispatcher::CurrentProcess();
    return ProcessManager::Fchdir(proc, fd);
}

int sys_uname(utsname *name)
{
    strcpy(name->sysname, "AndyOS");
    strcpy(name->nodename, "-");
    strcpy(name->release, "0.0.1");
    strcpy(name->version, "0.0.1");
    strcpy(name->machine, "x86");
    return 0;
}

int sys_ioctl(int fd, int request, unsigned int arg)
{
    kprintf("ioctl %d %p %p\n", fd, request, arg);
    return VFS::Ioctl(CurrentFiletable(), fd, request, arg);
}

//

int sys_getgroups(int size, gid_t list[])
{
    kprintf("Get groups %d\n", size);
    return 0;
}

int sys_getpgid(pid_t pid)
{
    PROCESS *current = Dispatcher::CurrentProcess();
    PROCESS *process = pid ? ProcessManager::GetProcess(pid) : current;

    if (process->sid != current->sid)
        return -EPERM;

    if (!process)
        return -ESRCH;

    return process->gid;
}

pid_t sys_getpid()
{
    return Dispatcher::CurrentProcess()->id;
}

pid_t sys_getppid()
{
    PROCESS *parent = Dispatcher::CurrentProcess()->parent;
    return parent ? parent->id : 0;
}

int sys_setpgid(pid_t pid, pid_t pgid)
{
    PROCESS *current = Dispatcher::CurrentProcess();
    PROCESS *process = pid ? ProcessManager::GetProcess(pid) : current;

    if (pgid < 0)
        return -EINVAL;

    if (process->sid != current->sid)
        return -EPERM;

    if (!process)
        return -ESRCH;

    process->gid = pgid ? pgid : current->sid;
    kprintf("Set pgid %d %d\n", process->id, process->gid);
    return 0;
}

//

int sys_socket(int domain, int type, int protocol)
{
    return VFS::CreateSocket(CurrentFiletable(), domain, type, protocol);
}

int sys_accept(int fd, struct sockaddr *addr, socklen_t addrlen, int flags)
{
    return VFS::SocketAccept(CurrentFiletable(), fd, addr, addrlen, flags);
}

int sys_bind(int fd, const sockaddr *addr, socklen_t addrlen)
{
    return VFS::SocketBind(CurrentFiletable(), fd, addr, addrlen);
}

int sys_connect(int fd, const struct sockaddr *addr, socklen_t addrlen)
{
    return VFS::SocketConnect(CurrentFiletable(), fd, addr, addrlen);
}

int sys_listen(int fd, int backlog)
{
    return VFS::SocketListen(CurrentFiletable(), fd, backlog);
}

int sys_recv(int fd, void *buf, size_t len, int flags)
{
    return VFS::SocketRecv(CurrentFiletable(), fd, buf, len, flags);
}

int sys_recvfrom(int fd, void *buf, size_t len, int flags, sockaddr *src_addr, socklen_t addrlen)
{
    return VFS::SocketRecvfrom(CurrentFiletable(), fd, buf, len, flags, src_addr, addrlen);
}

int sys_send(int fd, const void *buf, size_t len, int flags)
{
    return VFS::SocketSend(CurrentFiletable(), fd, buf, len, flags);
}

int sys_sendto(int fd, const void *buf, size_t len, int flags, const sockaddr *dest_addr,
               socklen_t addrlen)
{
    return VFS::SocketSendto(CurrentFiletable(), fd, buf, len, flags, dest_addr, addrlen);
}

int sys_shutdown(int fd, int how)
{
    return VFS::SocketShutdown(CurrentFiletable(), fd, how);
}

void sys_halt()
{
    sys_halt();
}

void sys_print(char *text)
{
    while (*text)
        debug_putc(*text++);
}

void sys_color(uint32 color)
{
    debug_color(color);
}

void sys_gettime(int &hour, int &minute, int &second)
{
    hour = RTC::Hour();
    minute = RTC::Minute();
    second = RTC::Second();
}

void sys_draw(uint32 *framebuffer)
{
    if (VTTY::CurrentTerminal()->tty->sid == Dispatcher::CurrentProcess()->sid)
        Video::mode->Draw(framebuffer);
}

void sys_exit(int code)
{
    ProcessManager::Exit(Dispatcher::CurrentProcess(), code);
}

int sys_thread_create(pthread_t *pthread, const pthread_attr_t *attr, void (*stub)(),
                      const void (*start_routine)(), void *arg)
{
    PROCESS *proc = Dispatcher::CurrentProcess();
    THREAD *thread = ProcessManager::CreateThread(proc, stub, start_routine, arg);

    if (!thread)
        return -1;

    if (pthread)
        *pthread = thread->id;

    Scheduler::InsertThread(thread);
    return 0;
}

void sys_thread_exit(int code)
{
    Scheduler::ExitThread(code, Dispatcher::CurrentThread());
}

int sys_sleep(useconds_t usec)
{
    uint64 sleep_until = Timer::Ticks() + usec;

    Scheduler::SleepThread(sleep_until, Scheduler::CurrentThread());
    uint64 now = Timer::Ticks();

    if (now < sleep_until)
        return -1;

    return 0;
}

uint32 sys_get_ticks()
{
    return Timer::Ticks();
}

bool sys_alloc_shared(int proc_id, void *&addr1, void *&addr2, uint32 blocks)
{
    ADDRESS_SPACE other_space = ProcessManager::GetProcess(proc_id)->addr_space;
    return VMem::UserAllocShared(other_space, addr1, addr2, blocks);
}

uint32 sys_read_file(char *&buffer, char *filename)
{
    char *kernel_buf;

    int size = VFS::ReadFile(filename, kernel_buf);
    buffer = (char *)VMem::UserAlloc(BYTES_TO_BLOCKS(size));

    memcpy(buffer, kernel_buf, size);
    return size;
}

pid_t sys_create_process(char *filename)
{
    PROCESS *proc = ProcessManager::Exec(filename);

    if (proc)
        return proc->id;

    return -1;
}

void sys_debug_reset()
{
    debug_clear(0xFF000000);
}

void InstallSyscall(int id, SYSCALL_HANDLER handler)
{
    if (id >= MAX_SYSCALLS)
        return;

    syscalls[id] = handler;
}

SYSCALL_HANDLER GetSyscall(int id)
{
    return syscalls[id];
}

void DoSyscall(DISPATCHER_CONTEXT &context, bool noreturn)
{
    void *location = (void *)Syscalls::GetSyscall(context.syscall);

    if (!location)
        panic("Invalid syscall", "Id: %i", context.syscall);

    THREAD *thread = context.thread;
    DISPATCHER_ENTRY *entry = Dispatcher::CurrentEntry();

    SYSCALL_FUNC syscall_func = (SYSCALL_FUNC)location;
    int ret = syscall_func(context.p1, context.p2, context.p3, context.p4, context.p5, context.p6);

    Scheduler::Disable();
    disable();
    thread->syscall_event.Set();
    enable();

    while (entry->handler_thread->signaled) {
        Scheduler::Enable();
        entry->handler_thread->signal_event.Wait();
        Scheduler::Disable();
    }

    if (!noreturn)
        Arch::ReturnSyscall(context, ret);

    Scheduler::Enable();
}

bool Init()
{
    memset(syscalls, 0, sizeof(syscalls));

    InstallSyscall(SYSCALL_EXIT, (SYSCALL_HANDLER)sys_exit);
    InstallSyscall(SYSCALL_OPEN, (SYSCALL_HANDLER)sys_open);
    InstallSyscall(SYSCALL_CLOSE, (SYSCALL_HANDLER)sys_close);
    InstallSyscall(SYSCALL_READ, (SYSCALL_HANDLER)sys_read);
    InstallSyscall(SYSCALL_WRITE, (SYSCALL_HANDLER)sys_write);
    InstallSyscall(SYSCALL_LSEEK, (SYSCALL_HANDLER)sys_lseek);
    InstallSyscall(SYSCALL_PIPE, (SYSCALL_HANDLER)sys_pipe);
    InstallSyscall(SYSCALL_DUP, (SYSCALL_HANDLER)sys_dup);
    InstallSyscall(SYSCALL_DUP2, (SYSCALL_HANDLER)sys_dup2);
    InstallSyscall(SYSCALL_FCNTL, (SYSCALL_HANDLER)sys_fcntl);
    InstallSyscall(SYSCALL_GETTIMEOFDAY, (SYSCALL_HANDLER)sys_gettimeofday);
    InstallSyscall(SYSCALL_GETDENTS, (SYSCALL_HANDLER)sys_getdents);
    InstallSyscall(SYSCALL_FORK, (SYSCALL_HANDLER)sys_fork);
    InstallSyscall(SYSCALL_EXECVE, (SYSCALL_HANDLER)sys_execve);
    InstallSyscall(SYSCALL_SBRK, (SYSCALL_HANDLER)sys_sbrk);
    InstallSyscall(SYSCALL_STAT, (SYSCALL_HANDLER)sys_stat);
    InstallSyscall(SYSCALL_FSTAT, (SYSCALL_HANDLER)sys_fstat);
    InstallSyscall(SYSCALL_KILL, (SYSCALL_HANDLER)sys_kill);
    InstallSyscall(SYSCALL_SIGACTION, (SYSCALL_HANDLER)sys_sigaction);
    InstallSyscall(SYSCALL_SIGPROCMASK, (SYSCALL_HANDLER)sys_sigprocmask);
    InstallSyscall(SYSCALL_SIGRETURN, (SYSCALL_HANDLER)sys_sigreturn);
    InstallSyscall(SYSCALL_SIGSUSPEND, (SYSCALL_HANDLER)sys_sigsuspend);
    InstallSyscall(SYSCALL_WAITPID, (SYSCALL_HANDLER)sys_waitpid);
    InstallSyscall(SYSCALL_CHDIR, (SYSCALL_HANDLER)sys_chdir);
    InstallSyscall(SYSCALL_FCHDIR, (SYSCALL_HANDLER)sys_fchdir);
    InstallSyscall(SYSCALL_UNAME, (SYSCALL_HANDLER)sys_uname);
    InstallSyscall(SYSCALL_IOCTL, (SYSCALL_HANDLER)sys_ioctl);

    InstallSyscall(SYSCALL_GETGROUPS, (SYSCALL_HANDLER)sys_getgroups);
    InstallSyscall(SYSCALL_GETPGID, (SYSCALL_HANDLER)sys_getpgid);
    InstallSyscall(SYSCALL_GETPID, (SYSCALL_HANDLER)sys_getpid);
    InstallSyscall(SYSCALL_GETPPID, (SYSCALL_HANDLER)sys_getppid);
    InstallSyscall(SYSCALL_SETPGID, (SYSCALL_HANDLER)sys_setpgid);

    InstallSyscall(SYSCALL_SOCKET, (SYSCALL_HANDLER)sys_socket);
    InstallSyscall(SYSCALL_ACCEPT, (SYSCALL_HANDLER)sys_accept);
    InstallSyscall(SYSCALL_BIND, (SYSCALL_HANDLER)sys_bind);
    InstallSyscall(SYSCALL_CONNECT, (SYSCALL_HANDLER)sys_connect);
    InstallSyscall(SYSCALL_LISTEN, (SYSCALL_HANDLER)sys_listen);
    InstallSyscall(SYSCALL_RECV, (SYSCALL_HANDLER)sys_recv);
    InstallSyscall(SYSCALL_RECVFROM, (SYSCALL_HANDLER)sys_recvfrom);
    InstallSyscall(SYSCALL_SEND, (SYSCALL_HANDLER)sys_send);
    InstallSyscall(SYSCALL_SENDTO, (SYSCALL_HANDLER)sys_sendto);
    InstallSyscall(SYSCALL_SHUTDOWN, (SYSCALL_HANDLER)sys_shutdown);

    InstallSyscall(SYSCALL_THREAD_CREATE, (SYSCALL_HANDLER)sys_thread_create);
    InstallSyscall(SYSCALL_THREAD_EXIT, (SYSCALL_HANDLER)sys_thread_exit);

    InstallSyscall(SYSCALL_HALT, (SYSCALL_HANDLER)sys_halt);
    InstallSyscall(SYSCALL_PRINT, (SYSCALL_HANDLER)sys_print);
    InstallSyscall(SYSCALL_COLOR, (SYSCALL_HANDLER)sys_color);
    InstallSyscall(SYSCALL_GETTIME, (SYSCALL_HANDLER)sys_gettime);
    InstallSyscall(SYSCALL_DRAW, (SYSCALL_HANDLER)sys_draw);
    InstallSyscall(SYSCALL_SLEEP, (SYSCALL_HANDLER)sys_sleep);
    InstallSyscall(SYSCALL_GET_TICKS, (SYSCALL_HANDLER)sys_get_ticks);
    InstallSyscall(SYSCALL_ALLOC_SHARED, (SYSCALL_HANDLER)sys_alloc_shared);
    InstallSyscall(SYSCALL_READ_FILE, (SYSCALL_HANDLER)sys_read_file);
    InstallSyscall(SYSCALL_CREATE_PROCESS, (SYSCALL_HANDLER)sys_create_process);
    InstallSyscall(SYSCALL_DEBUG_RESET, (SYSCALL_HANDLER)sys_debug_reset);

    return true;
}
} // namespace Syscalls
