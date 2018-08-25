#pragma once

#define SYSCALL_IRQ		0x80
#define MAX_SYSCALLS	1024

enum SYSCALLS
{
    SYSCALL_OPEN,
    SYSCALL_CLOSE,
    SYSCALL_READ,
    SYSCALL_WRITE,
    SYSCALL_SEEK,
    SYSCALL_PIPE,

    SYSCALL_DUP,
    SYSCALL_DUP2,

    SYSCALL_FORK,
    SYSCALL_GETPID,
    SYSCALL_EXECVE,

    SYSCALL_HALT,
    SYSCALL_PRINT,
    SYSCALL_COLOR,
    SYSCALL_GETTIME,
    SYSCALL_DRAW,
    SYSCALL_EXIT,
    SYSCALL_EXIT_THREAD,
    SYSCALL_SLEEP,
    SYSCALL_GET_TICKS,
    SYSCALL_GET_LAST_KEY,
    SYSCALL_ALLOC,
    SYSCALL_FREE,
    SYSCALL_ALLOC_SHARED,
    SYSCALL_READ_FILE,
    SYSCALL_CREATE_PROCESS,
    SYSCALL_DEBUG_RESET,

    SYSCALL_SET_SIGNAL,
    SYSCALL_SEND_SIGNAL,
    SYSCALL_SET_MESSAGE,
    SYSCALL_SEND_MESSAGE,
    SYSCALL_SEND_MESSAGE_RESPONSE,
    SYSCALL_GET_MESSAGE_RESPONSE
};