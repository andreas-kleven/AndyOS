#include <panic.h>
#include <types.h>

// https://wiki.osdev.org/Stack_Smashing_Protector

#if UINT32_MAX == UINTPTR_MAX
#define STACK_CHK_GUARD 0xe2dee396
#else
#define STACK_CHK_GUARD 0x595e9fbd94fda766
#endif

extern "C"
{
    uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

    __attribute__((noreturn)) void __stack_chk_fail()
    {
        panic("Stack smashing detected");
        __builtin_unreachable();
    }
}
