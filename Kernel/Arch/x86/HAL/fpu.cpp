#include <Arch/fpu.h>
#include <panic.h>
#include <string.h>

namespace FPU {

char __attribute__((aligned(16))) clean_state[512];

void SetTS()
{
    asm volatile("mov %cr0, %ecx\n"
                 "or $0x8, %ecx\n"
                 "mov %ecx, %cr0\n");
}

void ClearTS()
{
    asm volatile("mov %cr0, %ecx\n"
                 "and $0xFFFFFFF7, %ecx\n"
                 "mov %ecx, %cr0\n");
}

void CleanState(void *state)
{
    if ((size_t)state % 16)
        panic("FPU state not aligned", "%p %d\n", state, (size_t)state % 16);

    memcpy(state, clean_state, sizeof(clean_state));
}

STATUS Init()
{
    char __attribute__((aligned(16))) tmp[512];

    asm volatile("fxsave (%0)\n"
                 "finit\n"
                 "fxsave (%1)\n"
                 "fxrstor (%0)\n"
                 :
                 : "r"(tmp), "r"(clean_state));

    return STATUS_SUCCESS;
}

} // namespace FPU
