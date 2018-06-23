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