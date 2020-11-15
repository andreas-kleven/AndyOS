#include <Arch/process.h>
#include <Arch/regs.h>
#include <Process/thread.h>
#include <debug.h>
#include <hal.h>
#include <string.h>
#include <syscall_list.h>

namespace ProcessManager::Arch {
struct THREAD_SIGINFO
{
    uint32 eip;
    uint32 esp;
    uint32 eax;
    uint32 esi;
    THREAD_STATE state;
};

void HandleSignal(THREAD *thread, int signo, sig_t handler)
{
    uint8 instructions[] = {
        0x60, // pusha
        0x0f, 0xae,
        0x05, 0x00,
        0x00, 0x00,
        0x00,               // fxsave FPU_STATE
        0x6a, (uint8)signo, // push SIGNO
        0xbb, 0x00,
        0x00, 0x00,
        0x00,       // mob ebx, HANDLER
        0xff, 0xd3, // call ebx
        0x0f, 0xae,
        0x0d, 0x00,
        0x00, 0x00,
        0x00, // fxrstor FPU_STATE
        0x61, // popa
        0xbe, 0x00,
        0x00, 0x00,
        0x00, // mov esi, INFO
        0xb8, SYSCALL_SIGRETURN,
        0x0,  0x0,
        0x0,              // mov eax, SIGRETURN
        0xcd, SYSCALL_IRQ // int SYSCALL_IRQ
    };

    REGS *regs = (REGS *)thread->stack;
    uint32 *stack = (uint32 *)regs->user_stack;

    THREAD_SIGINFO *info = (THREAD_SIGINFO *)((size_t)stack - 1) - 1;
    uint32 *fpu_state = (uint32 *)((size_t)info - 512);
    fpu_state = (uint32 *)(((size_t)fpu_state / 16) * 16);
    uint32 *code = (uint32 *)((size_t)fpu_state - sizeof(instructions));

    info->eip = regs->eip;
    info->esp = regs->user_stack;
    info->eax = regs->eax;
    info->esi = regs->esi;
    info->state = thread->state;

    thread->state = THREAD_STATE_READY;

    *(uint32 *)&instructions[4] = (uint32)fpu_state;
    *(uint32 *)&instructions[11] = (uint32)handler;
    *(uint32 *)&instructions[20] = (uint32)fpu_state;
    *(uint32 *)&instructions[26] = (uint32)info;

    memcpy(code, instructions, sizeof(instructions));

    regs->user_stack = ((uint32)code / 16) * 16;
    regs->eip = (uint32)code;
}

void FinishSignal(THREAD *thread)
{
    REGS *regs = (REGS *)thread->stack;
    THREAD_SIGINFO *info = (THREAD_SIGINFO *)(regs->esi);

    regs->eip = info->eip;
    regs->user_stack = info->esp;
    regs->eax = info->eax;
    regs->esi = info->esi;
    thread->state = info->state;
}
}; // namespace ProcessManager::Arch
