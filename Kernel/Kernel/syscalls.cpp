#include "syscalls.h"
#include "Include/syscall_list.h"
#include "hal.h"
#include "irq.h"
#include "video.h"
#include "Kernel/timer.h"
#include "string.h"
#include "Lib/debug.h"
#include "Drivers/mouse.h"
#include "Drivers/keyboard.h"
#include "Drivers/rtc.h"
#include "Memory/memory.h"
#include "Process/scheduler.h"
#include "Process/process.h"
#include "FS/vfs.h"

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

	int open(const char *filename, int flags)
	{
		return VFS::Open(filename);
	}

	int close(int fd)
	{
		return VFS::Close(fd);
	}

	size_t read(int fd, char *buf, size_t size)
	{
		return VFS::Read(fd, buf, size);
	}

	size_t write(int fd, const char *buf, size_t size)
	{
		return VFS::Write(fd, buf, size);
	}

	off_t lseek(int fd, off_t offset, int whence)
	{
		return VFS::Seek(fd, offset, whence);
	}

	int pipe(int pipefd[2])
	{
		return VFS::CreatePipes(pipefd, 0);
	}

	int dup(int oldfd)
	{
		return VFS::DuplicateFile(oldfd);
	}

	int dup2(int oldfd, int newfd)
	{
		return VFS::DuplicateFile(oldfd, newfd);
	}

	pid_t fork()
	{
		PROCESS *newproc = ProcessManager::Fork(ProcessManager::GetCurrent());
		return newproc == 0 ? -1 : 0;
	}

	pid_t getpid()
	{
		return ProcessManager::GetCurrent()->id;
	}

	int execve(char const *path, char const *argv[], char const *envp[])
	{
		PROCESS *proc = ProcessManager::GetCurrent();
		return ProcessManager::Exec(proc, path, argv, envp);
	}

	void *sbrk(intptr_t increment)
	{
		PROCESS *proc = ProcessManager::GetCurrent();
		VMem::SwapAddressSpace(proc->addr_space);

		void *ret = 0;

		if (increment > 0)
		{
			ret = ProcessManager::HeapAlloc(proc, increment);
		}
		else if (increment > 0)
		{
			ret = ProcessManager::HeapFree(proc, -increment);
		}

		if (ret)
			return ret;

		return (void *)-1;
	}

	void halt()
	{
		halt();
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
		ProcessManager::Terminate(ProcessManager::GetCurrent());
	}

	void exit_thread(int code)
	{
		Scheduler::ExitThread(code, Scheduler::CurrentThread(), true);
	}

	void sleep(uint32 ticks)
	{
		Scheduler::SleepThread(Timer::Ticks() + ticks, Scheduler::CurrentThread());
	}

	uint32 get_ticks()
	{
		return Timer::Ticks();
	}

	bool get_last_key(KEYCODE &code, bool &pressed)
	{
		return Keyboard::GetLastKey(code, pressed);
	}

	void *alloc(uint32 blocks)
	{
		VMem::SwapAddressSpace(ProcessManager::GetCurrent()->addr_space);
		return VMem::UserAlloc(blocks);
	}

	void free(void *ptr, uint32 blocks)
	{
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

		return 0;
	}

	void debug_reset()
	{
		debug_clear(0xFF000000);
	}

	//
	int set_signal(SIGNAL_HANDLER handler)
	{
		ProcessManager::GetCurrent()->signal_handler = handler;
		return 1;
	}

	void send_signal(int proc_id, int signo)
	{
		PROCESS *proc = ProcessManager::GetProcess(proc_id);
		int src_proc = ProcessManager::GetCurrent()->id;

		if (proc)
		{
			MESSAGE msg(MESSAGE_TYPE_SIGNAL, ++msg_id, src_proc, signo, 0);
			proc->messages.Add(msg);
		}
	}

	int set_message(MESSAGE_HANDLER handler)
	{
		ProcessManager::GetCurrent()->message_handler = handler;
		return 1;
	}

	void send_message(int proc_id, int type, char *buf, int size, bool async, int &id)
	{
		PROCESS *proc = ProcessManager::GetProcess(proc_id);

		if (!proc)
			return;

		int src_proc = ProcessManager::GetCurrent()->id;
		id = ++msg_id;

		MESSAGE msg(MESSAGE_TYPE_MESSAGE, id, src_proc, type, size);
		msg.data = new char[size];
		memcpy(msg.data, buf, size);

		proc->messages.Add(msg);

		if (!async)
		{
			TMP_MSG *msg = new TMP_MSG;
			msg->id = id;
			msg->thread = Scheduler::CurrentThread();
			msg->next = first_msg;
			first_msg = msg;

			Scheduler::BlockThread(Scheduler::CurrentThread());
		}
	}

	void send_message_reponse(int msg_id, int type, char *buf, int size)
	{
		TMP_MSG *msg = first_msg;
		int src_proc = ProcessManager::GetCurrent()->id;

		while (msg)
		{
			if (msg->id == msg_id)
			{
				msg->received = true;
				msg->response = MESSAGE(MESSAGE_TYPE_RESPONSE, ++msg_id, src_proc, type, size);
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

	bool Init()
	{
		InstallSyscall(SYSCALL_OPEN, (SYSCALL_HANDLER)open);
		InstallSyscall(SYSCALL_CLOSE, (SYSCALL_HANDLER)close);
		InstallSyscall(SYSCALL_READ, (SYSCALL_HANDLER)read);
		InstallSyscall(SYSCALL_WRITE, (SYSCALL_HANDLER)write);
		InstallSyscall(SYSCALL_LSEEK, (SYSCALL_HANDLER)lseek);
		InstallSyscall(SYSCALL_PIPE, (SYSCALL_HANDLER)pipe);
		InstallSyscall(SYSCALL_DUP, (SYSCALL_HANDLER)dup);
		InstallSyscall(SYSCALL_DUP2, (SYSCALL_HANDLER)dup2);
		InstallSyscall(SYSCALL_FORK, (SYSCALL_HANDLER)fork);
		InstallSyscall(SYSCALL_GETPID, (SYSCALL_HANDLER)getpid);
		InstallSyscall(SYSCALL_EXECVE, (SYSCALL_HANDLER)execve);
		InstallSyscall(SYSCALL_SBRK, (SYSCALL_HANDLER)sbrk);

		InstallSyscall(SYSCALL_HALT, (SYSCALL_HANDLER)halt);
		InstallSyscall(SYSCALL_PRINT, (SYSCALL_HANDLER)print);
		InstallSyscall(SYSCALL_COLOR, (SYSCALL_HANDLER)color);
		InstallSyscall(SYSCALL_GETTIME, (SYSCALL_HANDLER)gettime);
		InstallSyscall(SYSCALL_DRAW, (SYSCALL_HANDLER)draw);
		InstallSyscall(SYSCALL_EXIT, (SYSCALL_HANDLER)exit);
		InstallSyscall(SYSCALL_EXIT_THREAD, (SYSCALL_HANDLER)exit_thread);
		InstallSyscall(SYSCALL_SLEEP, (SYSCALL_HANDLER)sleep);
		InstallSyscall(SYSCALL_GET_TICKS, (SYSCALL_HANDLER)get_ticks);
		InstallSyscall(SYSCALL_GET_LAST_KEY, (SYSCALL_HANDLER)get_last_key);
		InstallSyscall(SYSCALL_ALLOC, (SYSCALL_HANDLER)alloc);
		InstallSyscall(SYSCALL_FREE, (SYSCALL_HANDLER)free);
		InstallSyscall(SYSCALL_ALLOC_SHARED, (SYSCALL_HANDLER)alloc_shared);
		InstallSyscall(SYSCALL_READ_FILE, (SYSCALL_HANDLER)read_file);
		InstallSyscall(SYSCALL_CREATE_PROCESS, (SYSCALL_HANDLER)create_process);
		InstallSyscall(SYSCALL_DEBUG_RESET, (SYSCALL_HANDLER)debug_reset);

		InstallSyscall(SYSCALL_SET_SIGNAL, (SYSCALL_HANDLER)set_signal);
		InstallSyscall(SYSCALL_SEND_SIGNAL, (SYSCALL_HANDLER)send_signal);
		InstallSyscall(SYSCALL_SET_MESSAGE, (SYSCALL_HANDLER)set_message);
		InstallSyscall(SYSCALL_SEND_MESSAGE, (SYSCALL_HANDLER)send_message);
		InstallSyscall(SYSCALL_SEND_MESSAGE_RESPONSE, (SYSCALL_HANDLER)send_message_reponse);
		InstallSyscall(SYSCALL_GET_MESSAGE_RESPONSE, (SYSCALL_HANDLER)get_message_reponse);

		return true;
	}
} // namespace Syscalls