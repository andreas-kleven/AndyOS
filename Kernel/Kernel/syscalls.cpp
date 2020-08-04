#include <syscalls.h>
#include <Arch/syscalls.h>
#include <syscall_list.h>
#include <hal.h>
#include <irq.h>
#include <video.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <utsname.h>
#include <debug.h>
#include <Kernel/timer.h>
#include <Drivers/mouse.h>
#include <Drivers/keyboard.h>
#include <Drivers/rtc.h>
#include <memory.h>
#include <Process/dispatcher.h>
#include <Process/scheduler.h>
#include <Process/process.h>
#include <Net/socket.h>
#include <FS/vfs.h>

namespace Syscalls
{
	struct TMP_MSG
	{
		int id;
		THREAD *thread;
		TMP_MSG *next;

		bool received;
		MESSAGE response;

		TMP_MSG()
		{
			received = false;
		}
	};

	struct USER_MESSAGE
	{
		int id;
		int type;
		int size;
		char *data;
	};

	void *syscalls[MAX_SYSCALLS];
	int msg_id = 0;

	TMP_MSG *first_msg = 0;

	Filetable &CurrentFiletable()
	{
		return Dispatcher::CurrentProcess()->filetable;
	}

	int sys_open(const char *filename, int flags)
	{
		return VFS::Open(Dispatcher::CurrentProcess(), filename, flags);
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

	pid_t sys_getpid()
	{
		return Dispatcher::CurrentProcess()->id;
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
		// TODO
		if (pid < 0)
			pid = -pid;

		PROCESS *proc = ProcessManager::GetProcess(pid);
		return ProcessManager::HandleSignal(proc, signo);
	}

	sig_t sys_signal(int signum, sig_t handler)
	{
		PROCESS *proc = Dispatcher::CurrentProcess();
		return ProcessManager::SetSignalHandler(proc, signum, handler);
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
		debug_print("ioctl %d %p %p\n", fd, request, arg);
		return VFS::Ioctl(CurrentFiletable(), fd, request, arg);
	}

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

	int sys_sendto(int fd, const void *buf, size_t len, int flags, const sockaddr *dest_addr, socklen_t addrlen)
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
		Video::Draw(framebuffer);
	}

	void sys_exit(int code)
	{
		ProcessManager::Exit(Dispatcher::CurrentProcess(), code);
	}

	void sys_exit_thread(int code)
	{
		Scheduler::ExitThread(code, Dispatcher::CurrentThread());
	}

	int sys_sleep(useconds_t usec)
	{
		Scheduler::SleepThread(Timer::Ticks() + usec, Dispatcher::CurrentThread());
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

	//
	int sys_set_message(MESSAGE_HANDLER handler)
	{
		Dispatcher::CurrentProcess()->message_handler = handler;
		return 1;
	}

	void sys_send_message(int proc_id, int type, char *buf, int size, bool async, int &id)
	{
		PROCESS *src_proc = Dispatcher::CurrentProcess();
		PROCESS *dst_proc = ProcessManager::GetProcess(proc_id);

		if (!dst_proc)
			return;

		id = ++msg_id;

		MESSAGE msg(MESSAGE_TYPE_MESSAGE, id, src_proc->id, type, size);
		msg.data = new char[size];
		memcpy(msg.data, buf, size);

		dst_proc->messages.Add(msg);

		if (!async)
		{
			TMP_MSG *msg = new TMP_MSG;
			msg->id = id;
			msg->thread = Scheduler::CurrentThread();
			msg->next = first_msg;
			first_msg = msg;

			Scheduler::BlockThread(msg->thread);
		}
	}

	void sys_send_message_reponse(int msg_id, int type, char *buf, int size)
	{
		PROCESS *src_proc = Dispatcher::CurrentProcess();
		TMP_MSG *msg = first_msg;

		while (msg)
		{
			if (msg->id == msg_id)
			{
				msg->received = true;
				msg->response = MESSAGE(MESSAGE_TYPE_RESPONSE, ++msg_id, src_proc->id, type, size);
				memcpy(msg->response.data, buf, size);

				Scheduler::WakeThread(msg->thread);
				break;
			}

			msg = msg->next;
		}
	}

	bool sys_get_message_reponse(int msg_id, USER_MESSAGE &response)
	{
		TMP_MSG *msg = first_msg;
		TMP_MSG *prev = 0;

		while (msg)
		{
			if (msg->id == msg_id)
			{
				if (msg->received)
				{
					response.id = ++msg->id;
					response.type = msg->response.type;
					response.size = msg->response.size;

					response.data = (char *)VMem::UserAlloc(BYTES_TO_BLOCKS(response.size));
					memcpy(response.data, msg->response.data, response.size);

					//Remove message
					if (prev)
					{
						prev->next = msg->next;
					}
					else
					{
						first_msg = msg->next;
					}

					delete msg;
					return true;
				}

				break;
			}

			prev = msg;
			msg = msg->next;
		}

		return false;
	}

	void InstallSyscall(int id, SYSCALL_HANDLER handler)
	{
		if (id >= MAX_SYSCALLS)
			return;

		syscalls[id] = (void *)handler;
	}

	SYSCALL_HANDLER GetSyscall(int id)
	{
		return (SYSCALL_HANDLER)syscalls[id];
	}

	void DoSyscall(DISPATCHER_CONTEXT &context)
	{
		void *location = (void *)Syscalls::GetSyscall(context.syscall);

		if (!location)
			panic("Invalid syscall", "Id: %i", context.syscall);

		Arch::DoSyscall(context, location);
	}

	bool Init()
	{
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
		InstallSyscall(SYSCALL_GETPID, (SYSCALL_HANDLER)sys_getpid);
		InstallSyscall(SYSCALL_EXECVE, (SYSCALL_HANDLER)sys_execve);
		InstallSyscall(SYSCALL_SBRK, (SYSCALL_HANDLER)sys_sbrk);
		InstallSyscall(SYSCALL_STAT, (SYSCALL_HANDLER)sys_stat);
		InstallSyscall(SYSCALL_FSTAT, (SYSCALL_HANDLER)sys_fstat);
		InstallSyscall(SYSCALL_KILL, (SYSCALL_HANDLER)sys_kill);
		InstallSyscall(SYSCALL_SIGNAL, (SYSCALL_HANDLER)sys_signal);
		InstallSyscall(SYSCALL_WAITPID, (SYSCALL_HANDLER)sys_waitpid);
		InstallSyscall(SYSCALL_CHDIR, (SYSCALL_HANDLER)sys_chdir);
		InstallSyscall(SYSCALL_FCHDIR, (SYSCALL_HANDLER)sys_fchdir);
		InstallSyscall(SYSCALL_UNAME, (SYSCALL_HANDLER)sys_uname);
		InstallSyscall(SYSCALL_IOCTL, (SYSCALL_HANDLER)sys_ioctl);

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

		InstallSyscall(SYSCALL_HALT, (SYSCALL_HANDLER)sys_halt);
		InstallSyscall(SYSCALL_PRINT, (SYSCALL_HANDLER)sys_print);
		InstallSyscall(SYSCALL_COLOR, (SYSCALL_HANDLER)sys_color);
		InstallSyscall(SYSCALL_GETTIME, (SYSCALL_HANDLER)sys_gettime);
		InstallSyscall(SYSCALL_DRAW, (SYSCALL_HANDLER)sys_draw);
		InstallSyscall(SYSCALL_EXIT_THREAD, (SYSCALL_HANDLER)sys_exit_thread);
		InstallSyscall(SYSCALL_SLEEP, (SYSCALL_HANDLER)sys_sleep);
		InstallSyscall(SYSCALL_GET_TICKS, (SYSCALL_HANDLER)sys_get_ticks);
		InstallSyscall(SYSCALL_ALLOC_SHARED, (SYSCALL_HANDLER)sys_alloc_shared);
		InstallSyscall(SYSCALL_READ_FILE, (SYSCALL_HANDLER)sys_read_file);
		InstallSyscall(SYSCALL_CREATE_PROCESS, (SYSCALL_HANDLER)sys_create_process);
		InstallSyscall(SYSCALL_DEBUG_RESET, (SYSCALL_HANDLER)sys_debug_reset);

		InstallSyscall(SYSCALL_SET_MESSAGE, (SYSCALL_HANDLER)sys_set_message);
		InstallSyscall(SYSCALL_SEND_MESSAGE, (SYSCALL_HANDLER)sys_send_message);
		InstallSyscall(SYSCALL_SEND_MESSAGE_RESPONSE, (SYSCALL_HANDLER)sys_send_message_reponse);
		InstallSyscall(SYSCALL_GET_MESSAGE_RESPONSE, (SYSCALL_HANDLER)sys_get_message_reponse);

		return true;
	}
} // namespace Syscalls