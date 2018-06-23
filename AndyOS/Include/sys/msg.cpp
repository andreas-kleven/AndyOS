#include "msg.h"
#include "../../API/syscalls.h"

int set_signal(void(*handler)(int))
{
    return Call(SYSCALL_SET_SIGNAL, (int)handler);
}

void send_signal(int proc_id, int signo)
{
    Call(SYSCALL_SEND_SIGNAL, proc_id, signo);
}

int set_message(void(*handler)(int, char*, int))
{
    return Call(SYSCALL_SET_MESSAGE, (int)handler);
}

void send_message(int proc_id, int type, char* buf, int size)
{
    Call(SYSCALL_SEND_MESSAGE, proc_id, type, (int)buf, size);
}