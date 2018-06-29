#include "syscalls.h"
#include "Include/syscall_list.h"
#include "HAL/hal.h"
#include "string.h"
#include "Lib/debug.h"
#include "Drawing/vbe.h"
#include "Drivers/mouse.h"
#include "Drivers/keyboard.h"
#include "Memory/memory.h"
#include "Process/scheduler.h"
#include "Process/process.h"
#include "FS/vfs.h"

struct TMP_MSG
{
	int id;
	THREAD* thread;
	TMP_MSG* next;

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
    char* data;
};

static void* syscalls[MAX_SYSCALLS];
static int msg_id = 0;

static TMP_MSG* first_msg = 0;

void halt()
{
	asm volatile(
		"cli\n"
		"hlt");
}

void print(char* text)
{
	Debug::Print(text);
}

void color(uint32 color)
{
	Debug::color = color;
}

void gettime(int& hour, int& minute, int& second)
{
	hour = RTC::Hour();
	minute = RTC::Minute();
	second = RTC::Second();
}

void draw(uint32* framebuffer)
{
	memcpy(VBE::mem_base, framebuffer, VBE::mem_size);
}

void exit(int code)
{
	Scheduler::ExitThread(code);
}

void sleep(uint32 ticks)
{
	Scheduler::SleepThread(PIT::ticks + ticks);
}

uint32 get_ticks()
{
	return PIT::ticks;
}

void get_mouse_pos(int& x, int& y)
{
	x = Mouse::x;
	y = Mouse::y;
}

void get_mouse_buttons(bool& left, bool& right, bool& middle)
{
	left = Mouse::mouse_L;
	right = Mouse::mouse_R;
	middle = Mouse::mouse_M;
}

bool get_last_key(KEYCODE& code, bool& pressed)
{
	return Keyboard::GetLastKey(code, pressed);
}

void* alloc(uint32 blocks)
{
	VMem::SwitchDir(Scheduler::current_thread->page_dir);
	return VMem::UserAlloc(blocks);
}

void free(void* ptr, uint32 blocks)
{

}

void alloc_shared(int proc_id, void*& addr1, void*& addr2, uint32 blocks)
{
	PAGE_DIR* dir1 = Scheduler::current_thread->page_dir;
	PAGE_DIR* dir2 = ProcessManager::GetProcess(proc_id)->page_dir;
	VMem::UserAllocShared(dir1, dir2, addr1, addr2, blocks);
}

uint32 read_file(char*& buffer, char* filename)
{
	char* kernel_buf;

	int size = VFS::ReadFile(filename, kernel_buf);
	buffer = (char*)VMem::UserAlloc(BYTES_TO_BLOCKS(size));

	memcpy(buffer, kernel_buf, size);
	return size;
}

void debug_reset()
{
	Debug::Clear(0xFF000000);
}

//
int set_signal(SIGNAL_HANDLER handler)
{
	Scheduler::current_thread->process->signal_handler = handler;
	return 1;
}

void send_signal(int proc_id, int signo)
{
	PROCESS* proc = ProcessManager::GetProcess(proc_id);
	int src_proc = Scheduler::current_thread->process->id;

	if (proc)
	{
		MESSAGE msg(MESSAGE_TYPE_SIGNAL, ++msg_id, src_proc, signo, 0);
		proc->messages.Add(msg);
	}
}

int set_message(MESSAGE_HANDLER handler)
{
	Scheduler::current_thread->process->message_handler = handler;
	return 1;
}

void send_message(int proc_id, int type, char* buf, int size, bool async, int& id)
{
	PROCESS* proc = ProcessManager::GetProcess(proc_id);

	if (!proc)
		return;

	int src_proc = Scheduler::current_thread->process->id;
	id = ++msg_id;

	MESSAGE msg(MESSAGE_TYPE_MESSAGE, id, src_proc, type, size);
	msg.data = new char[size];
	memcpy(msg.data, buf, size);

	proc->messages.Add(msg);

	if (!async)
	{
		TMP_MSG* msg = new TMP_MSG;
		msg->id = id;
		msg->thread = Scheduler::current_thread;
		msg->next = first_msg;
		first_msg = msg;

		Scheduler::BlockThread(Scheduler::current_thread);
	}
}

void send_message_reponse(int msg_id, int type, char* buf, int size)
{
	TMP_MSG* msg = first_msg;
	int src_proc = Scheduler::current_thread->process->id;

	while (msg)
	{
		if (msg->id == msg_id)
		{
			msg->received = true;
			msg->response = MESSAGE(MESSAGE_TYPE_RESPONSE, ++msg_id, src_proc, type, size);
			memcpy(msg->response.data, buf, size);

			Scheduler::AwakeThread(msg->thread);
			break;
		}

		msg = msg->next;
	}
}

bool get_message_reponse(int msg_id, USER_MESSAGE& response)
{
	TMP_MSG* msg = first_msg;
	TMP_MSG* prev = 0;

	while (msg)
	{
		if (msg->id == msg_id)
		{
			if (msg->received)
			{
				response.id = ++msg->id;
				response.type = msg->response.type;
				response.size = msg->response.size;

				response.data = (char*)VMem::UserAlloc(BYTES_TO_BLOCKS(response.size));
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

//
STATUS Syscalls::Init()
{
	if (!IDT::InstallIRQ(SYSCALL_IRQ, (IRQ_HANDLER)ISR))
		return STATUS_FAILED;

	InstallSyscall(SYSCALL_HALT, (SYSCALL_HANDLER)halt);
	InstallSyscall(SYSCALL_PRINT, (SYSCALL_HANDLER)print);
	InstallSyscall(SYSCALL_COLOR, (SYSCALL_HANDLER)color);
	InstallSyscall(SYSCALL_GETTIME, (SYSCALL_HANDLER)gettime);
	InstallSyscall(SYSCALL_DRAW, (SYSCALL_HANDLER)draw);
	InstallSyscall(SYSCALL_EXIT, (SYSCALL_HANDLER)exit);
	InstallSyscall(SYSCALL_SLEEP, (SYSCALL_HANDLER)sleep);
	InstallSyscall(SYSCALL_GET_TICKS, (SYSCALL_HANDLER)get_ticks);
	InstallSyscall(SYSCALL_GET_MOUSE_POS, (SYSCALL_HANDLER)get_mouse_pos);
	InstallSyscall(SYSCALL_GET_MOUSE_BUTTONS, (SYSCALL_HANDLER)get_mouse_buttons);
	InstallSyscall(SYSCALL_GET_LAST_KEY, (SYSCALL_HANDLER)get_last_key);
	InstallSyscall(SYSCALL_ALLOC, (SYSCALL_HANDLER)alloc);
	InstallSyscall(SYSCALL_FREE, (SYSCALL_HANDLER)free);
	InstallSyscall(SYSCALL_ALLOC_SHARED, (SYSCALL_HANDLER)alloc_shared);
	InstallSyscall(SYSCALL_READ_FILE, (SYSCALL_HANDLER)read_file);
	InstallSyscall(SYSCALL_DEBUG_RESET, (SYSCALL_HANDLER)debug_reset);

	InstallSyscall(SYSCALL_SET_SIGNAL, (SYSCALL_HANDLER)set_signal);
	InstallSyscall(SYSCALL_SEND_SIGNAL, (SYSCALL_HANDLER)send_signal);
	InstallSyscall(SYSCALL_SET_MESSAGE, (SYSCALL_HANDLER)set_message);
	InstallSyscall(SYSCALL_SEND_MESSAGE, (SYSCALL_HANDLER)send_message);
	InstallSyscall(SYSCALL_SEND_MESSAGE_RESPONSE, (SYSCALL_HANDLER)send_message_reponse);
	InstallSyscall(SYSCALL_GET_MESSAGE_RESPONSE, (SYSCALL_HANDLER)get_message_reponse);
}

void Syscalls::InstallSyscall(int id, SYSCALL_HANDLER handler)
{
	if (id >= MAX_SYSCALLS)
		return;

	syscalls[id] = (void*)handler;
}

void Syscalls::ISR(REGS* regs)
{
	if (regs->eax > sizeof(syscalls))
		return;

	void* location = syscalls[regs->eax];

	if (!location)
		return;

	uint32 ret;

	asm volatile (
		"push %1\n"
		"push %2\n"
		"push %3\n"
		"push %4\n"
		"push %5\n"
		"push %6\n"
		"call *%7\n"
		"pop %%ebx\n"
		"pop %%ebx\n"
		"pop %%ebx\n"
		"pop %%ebx\n"
		"pop %%ebx\n"
		"pop %%ebx\n"
		: "=a" (ret) : "g" (regs->ebp), "r" (regs->edi), "r" (regs->esi), "r" (regs->edx), "r" (regs->ecx), "r" (regs->ebx), "r" (location));

	regs->eax = ret;
}
