#pragma once

#define SYSCALL_IRQ		0x80
#define MAX_SYSCALLS	1024

enum SYSCALLS
{
    SYSCALL_HALT,
    SYSCALL_PRINT,
    SYSCALL_COLOR,
    SYSCALL_GETTIME,
    SYSCALL_DRAW,
    SYSCALL_EXIT,
    SYSCALL_SLEEP,
    SYSCALL_GET_TICKS,
    SYSCALL_GET_MOUSE_POS,
    SYSCALL_GET_MOUSE_BUTTONS,
    SYSCALL_GET_KEY_DOWN,
    SYSCALL_ALLOC,
    SYSCALL_FREE,
    SYSCALL_READ_FILE,
    SYSCALL_DEBUG_RESET,
    SYSCALL_SET_SIGNAL,
    SYSCALL_SEND_SIGNAL
};