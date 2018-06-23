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

void* syscalls[MAX_SYSCALLS];

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

bool get_key_down(KEYCODE key)
{
	return Keyboard::GetKeyDown(key);
}

void* alloc(uint32 blocks)
{
	VMem::SwitchDir(Scheduler::current_thread->page_dir);
	return VMem::UserAlloc(blocks);
}

void free(void* ptr, uint32 blocks)
{

}

uint32 read_file(char** buffer, char* filename)
{
	return VFS::ReadFile(filename, *buffer);
}

void debug_reset()
{
	Debug::Clear(0xFF000000);
}

int set_signal(SIGNAL_HANDLER handler)
{
	Scheduler::current_thread->process->signal_handler = handler;
	return 1;
}

void send_signal(int proc_id, int signo)
{
	PROCESS* proc = ProcessManager::GetProcess(proc_id);

	if (proc)
	{
		MESSAGE msg;
		msg.type = MESSAGE_TYPE_SIGNAL;
		msg.signo = signo;

		proc->messages.Add(msg);
	}
}

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
	InstallSyscall(SYSCALL_GET_KEY_DOWN, (SYSCALL_HANDLER)get_key_down);
	InstallSyscall(SYSCALL_ALLOC, (SYSCALL_HANDLER)alloc);
	InstallSyscall(SYSCALL_FREE, (SYSCALL_HANDLER)free);
	InstallSyscall(SYSCALL_READ_FILE, (SYSCALL_HANDLER)read_file);
	InstallSyscall(SYSCALL_DEBUG_RESET, (SYSCALL_HANDLER)debug_reset);
	InstallSyscall(SYSCALL_SET_SIGNAL, (SYSCALL_HANDLER)set_signal);
	InstallSyscall(SYSCALL_SEND_SIGNAL, (SYSCALL_HANDLER)send_signal);
}

void Syscalls::InstallSyscall(int id, SYSCALL_HANDLER handler)
{
	if (id <= 0 || id >= MAX_SYSCALLS)
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
		"call *%6\n"
		"pop %%ebx\n"
		"pop %%ebx\n"
		"pop %%ebx\n"
		"pop %%ebx\n"
		"pop %%ebx\n"
		: "=a" (ret) : "r" (regs->edi), "r" (regs->esi), "r" (regs->edx), "r" (regs->ecx), "r" (regs->ebx), "r" (location));

	regs->eax = ret;
}
