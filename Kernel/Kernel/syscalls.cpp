#include <syscalls.h>
#include <Arch/syscalls.h>
#include <syscall_list.h>
#include <hal.h>
#include <irq.h>
#include <video.h>
#include <string.h>
#include <errno.h>
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

	int open(const char *filename, int flags)
	{
		return VFS::Open(CurrentFiletable(), filename);
	}

	int close(int fd)
	{
		return VFS::Close(CurrentFiletable(), fd);
	}

	size_t read(int fd, char *buf, size_t size)
	{
		return VFS::Read(CurrentFiletable(), fd, buf, size);
	}

	size_t write(int fd, const char *buf, size_t size)
	{
		return VFS::Write(CurrentFiletable(), fd, buf, size);
	}

	off_t lseek(int fd, off_t offset, int whence)
	{
		return VFS::Seek(CurrentFiletable(), fd, offset, whence);
	}

	int pipe(int pipefd[2])
	{
		return VFS::CreatePipes(CurrentFiletable(), pipefd, 0);
	}

	int dup(int oldfd)
	{
		return VFS::DuplicateFile(CurrentFiletable(), oldfd);
	}

	int dup2(int oldfd, int newfd)
	{
		return VFS::DuplicateFile(CurrentFiletable(), oldfd, newfd);
	}

	int fcntl(int fd, int cmd, void *arg)
	{
		return VFS::Fcntl(CurrentFiletable(), fd, cmd, arg);
	}

	int getdents(int fd, dirent *dirp, unsigned int count)
	{
		return VFS::Getdents(CurrentFiletable(), fd, dirp, count);
	}

	pid_t fork()
	{
		PROCESS *newproc = ProcessManager::Fork(Dispatcher::CurrentProcess());
		return newproc ? newproc->id : -1;
	}

	pid_t getpid()
	{
		return Dispatcher::CurrentProcess()->id;
	}

	int execve(char const *path, char const *argv[], char const *envp[])
	{
		PROCESS *proc = Dispatcher::CurrentProcess();
		return ProcessManager::Exec(proc, path, argv, envp);
	}

	void *sbrk(intptr_t increment)
	{
		PROCESS *proc = Dispatcher::CurrentProcess();
		void *ret = ProcessManager::AdjustHeap(proc, increment);

		if (ret)
			return ret;

		return (void *)-1;
	}

	int fd_stat(const char *file, stat *st)
	{
		if (file && !VFS::GetDentry(Path(file)))
			return -ENOENT;

		st->st_dev = 1;
		st->st_ino = 1;
		st->st_mode = 1;
		st->st_nlink = 0;
		st->st_uid = 0;
		st->st_gid = 0;
		st->st_rdev = 0;
		st->st_size = 100;
		st->st_blksize = 512;
		st->st_blocks = 1;
		st->st_atime = 0;
		st->st_mtime = 0;
		st->st_ctime = 0;
		return 0;
	}

	int fstat(int fd, stat *st)
	{
		return fd_stat(0, st);
	}

	int kill(pid_t pid, int signo)
	{
		// TODO
		if (pid < 0)
			pid = -pid;

		PROCESS *proc = ProcessManager::GetProcess(pid);
		return ProcessManager::HandleSignal(proc, signo);
	}

	sig_t signal(int signum, sig_t handler)
	{
		PROCESS *proc = Dispatcher::CurrentProcess();
		return ProcessManager::SetSignalHandler(proc, signum, handler);
	}

	int waitpid(pid_t pid, int *status, int options)
	{
		return Dispatcher::Waitpid(pid, status, options);
	}

	int socket(int domain, int type, int protocol)
	{
		return VFS::CreateSocket(CurrentFiletable(), domain, type, protocol);
	}

	int accept(int fd, struct sockaddr *addr, socklen_t addrlen, int flags)
	{
		return VFS::SocketAccept(CurrentFiletable(), fd, addr, addrlen, flags);
	}

	int bind(int fd, const sockaddr *addr, socklen_t addrlen)
	{
		return VFS::SocketBind(CurrentFiletable(), fd, addr, addrlen);
	}

	int connect(int fd, const struct sockaddr *addr, socklen_t addrlen)
	{
		return VFS::SocketConnect(CurrentFiletable(), fd, addr, addrlen);
	}

	int listen(int fd, int backlog)
	{
		return VFS::SocketListen(CurrentFiletable(), fd, backlog);
	}

	int recv(int fd, void *buf, size_t len, int flags)
	{
		return VFS::SocketRecv(CurrentFiletable(), fd, buf, len, flags);
	}

	int recvfrom(int fd, void *buf, size_t len, int flags, sockaddr *src_addr, socklen_t addrlen)
	{
		return VFS::SocketRecvfrom(CurrentFiletable(), fd, buf, len, flags, src_addr, addrlen);
	}

	int send(int fd, const void *buf, size_t len, int flags)
	{
		return VFS::SocketSend(CurrentFiletable(), fd, buf, len, flags);
	}

	int sendto(int fd, const void *buf, size_t len, int flags, const sockaddr *dest_addr, socklen_t addrlen)
	{
		return VFS::SocketSendto(CurrentFiletable(), fd, buf, len, flags, dest_addr, addrlen);
	}

	int shutdown(int fd, int how)
	{
		return VFS::SocketShutdown(CurrentFiletable(), fd, how);
	}

	void halt()
	{
		sys_halt();
	}

	void print(char *text)
	{
		while (*text)
			debug_putc(*text++);
	}

	void color(uint32 color)
	{
		debug_color(color);
	}

	void gettime(int &hour, int &minute, int &second)
	{
		hour = RTC::Hour();
		minute = RTC::Minute();
		second = RTC::Second();
	}

	void draw(uint32 *framebuffer)
	{
		Video::Draw(framebuffer);
	}

	void exit(int code)
	{
		ProcessManager::Exit(Dispatcher::CurrentProcess(), code);
	}

	void exit_thread(int code)
	{
		Scheduler::ExitThread(code, Dispatcher::CurrentThread());
	}

	int sleep(useconds_t usec)
	{
		Scheduler::SleepThread(Timer::Ticks() + usec, Dispatcher::CurrentThread());
		return 0;
	}

	uint32 get_ticks()
	{
		return Timer::Ticks();
	}

	bool get_last_key(KEYCODE &code, bool &pressed)
	{
		return Keyboard::GetLastKey(code, pressed);
	}

	bool alloc_shared(int proc_id, void *&addr1, void *&addr2, uint32 blocks)
	{
		ADDRESS_SPACE other_space = ProcessManager::GetProcess(proc_id)->addr_space;
		return VMem::UserAllocShared(other_space, addr1, addr2, blocks);
	}

	uint32 read_file(char *&buffer, char *filename)
	{
		char *kernel_buf;

		int size = VFS::ReadFile(filename, kernel_buf);
		buffer = (char *)VMem::UserAlloc(BYTES_TO_BLOCKS(size));

		memcpy(buffer, kernel_buf, size);
		return size;
	}

	pid_t create_process(char *filename)
	{
		PROCESS *proc = ProcessManager::Exec(filename);

		if (proc)
			return proc->id;

		return -1;
	}

	void debug_reset()
	{
		debug_clear(0xFF000000);
	}

	//
	int set_message(MESSAGE_HANDLER handler)
	{
		Dispatcher::CurrentProcess()->message_handler = handler;
		return 1;
	}

	void send_message(int proc_id, int type, char *buf, int size, bool async, int &id)
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

	void send_message_reponse(int msg_id, int type, char *buf, int size)
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

	bool get_message_reponse(int msg_id, USER_MESSAGE &response)
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
		InstallSyscall(SYSCALL_EXIT, (SYSCALL_HANDLER)exit);
		InstallSyscall(SYSCALL_OPEN, (SYSCALL_HANDLER)open);
		InstallSyscall(SYSCALL_CLOSE, (SYSCALL_HANDLER)close);
		InstallSyscall(SYSCALL_READ, (SYSCALL_HANDLER)read);
		InstallSyscall(SYSCALL_WRITE, (SYSCALL_HANDLER)write);
		InstallSyscall(SYSCALL_LSEEK, (SYSCALL_HANDLER)lseek);
		InstallSyscall(SYSCALL_PIPE, (SYSCALL_HANDLER)pipe);
		InstallSyscall(SYSCALL_DUP, (SYSCALL_HANDLER)dup);
		InstallSyscall(SYSCALL_DUP2, (SYSCALL_HANDLER)dup2);
		InstallSyscall(SYSCALL_FCNTL, (SYSCALL_HANDLER)fcntl);
		InstallSyscall(SYSCALL_GETDENTS, (SYSCALL_HANDLER)getdents);
		InstallSyscall(SYSCALL_FORK, (SYSCALL_HANDLER)fork);
		InstallSyscall(SYSCALL_GETPID, (SYSCALL_HANDLER)getpid);
		InstallSyscall(SYSCALL_EXECVE, (SYSCALL_HANDLER)execve);
		InstallSyscall(SYSCALL_SBRK, (SYSCALL_HANDLER)sbrk);
		InstallSyscall(SYSCALL_STAT, (SYSCALL_HANDLER)fd_stat);
		InstallSyscall(SYSCALL_FSTAT, (SYSCALL_HANDLER)fstat);
		InstallSyscall(SYSCALL_KILL, (SYSCALL_HANDLER)kill);
		InstallSyscall(SYSCALL_SIGNAL, (SYSCALL_HANDLER)signal);
		InstallSyscall(SYSCALL_WAITPID, (SYSCALL_HANDLER)waitpid);

		InstallSyscall(SYSCALL_SOCKET, (SYSCALL_HANDLER)socket);
		InstallSyscall(SYSCALL_ACCEPT, (SYSCALL_HANDLER)accept);
		InstallSyscall(SYSCALL_BIND, (SYSCALL_HANDLER)bind);
		InstallSyscall(SYSCALL_CONNECT, (SYSCALL_HANDLER)connect);
		InstallSyscall(SYSCALL_LISTEN, (SYSCALL_HANDLER)listen);
		InstallSyscall(SYSCALL_RECV, (SYSCALL_HANDLER)recv);
		InstallSyscall(SYSCALL_RECVFROM, (SYSCALL_HANDLER)recvfrom);
		InstallSyscall(SYSCALL_SEND, (SYSCALL_HANDLER)send);
		InstallSyscall(SYSCALL_SENDTO, (SYSCALL_HANDLER)sendto);
		InstallSyscall(SYSCALL_SHUTDOWN, (SYSCALL_HANDLER)shutdown);

		InstallSyscall(SYSCALL_HALT, (SYSCALL_HANDLER)halt);
		InstallSyscall(SYSCALL_PRINT, (SYSCALL_HANDLER)print);
		InstallSyscall(SYSCALL_COLOR, (SYSCALL_HANDLER)color);
		InstallSyscall(SYSCALL_GETTIME, (SYSCALL_HANDLER)gettime);
		InstallSyscall(SYSCALL_DRAW, (SYSCALL_HANDLER)draw);
		InstallSyscall(SYSCALL_EXIT_THREAD, (SYSCALL_HANDLER)exit_thread);
		InstallSyscall(SYSCALL_SLEEP, (SYSCALL_HANDLER)sleep);
		InstallSyscall(SYSCALL_GET_TICKS, (SYSCALL_HANDLER)get_ticks);
		InstallSyscall(SYSCALL_GET_LAST_KEY, (SYSCALL_HANDLER)get_last_key);
		InstallSyscall(SYSCALL_ALLOC_SHARED, (SYSCALL_HANDLER)alloc_shared);
		InstallSyscall(SYSCALL_READ_FILE, (SYSCALL_HANDLER)read_file);
		InstallSyscall(SYSCALL_CREATE_PROCESS, (SYSCALL_HANDLER)create_process);
		InstallSyscall(SYSCALL_DEBUG_RESET, (SYSCALL_HANDLER)debug_reset);

		InstallSyscall(SYSCALL_SET_MESSAGE, (SYSCALL_HANDLER)set_message);
		InstallSyscall(SYSCALL_SEND_MESSAGE, (SYSCALL_HANDLER)send_message);
		InstallSyscall(SYSCALL_SEND_MESSAGE_RESPONSE, (SYSCALL_HANDLER)send_message_reponse);
		InstallSyscall(SYSCALL_GET_MESSAGE_RESPONSE, (SYSCALL_HANDLER)get_message_reponse);

		return true;
	}
} // namespace Syscalls