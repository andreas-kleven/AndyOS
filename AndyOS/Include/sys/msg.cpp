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

int set_message(void(*handler)(int, int, char*, int))
{
    return Call(SYSCALL_SET_MESSAGE, (int)handler);
}

void post_message(int proc_id, int type, void* data, int size)
{
    //Asynchronous
    int msg_id;
    Call(SYSCALL_SEND_MESSAGE, proc_id, type, (int)data, size, true, (int)&msg_id);
}

MESSAGE send_message(int proc_id, int type, void* data, int size)
{
    //Synchronous
    int msg_id;
    Call(SYSCALL_SEND_MESSAGE, proc_id, type, (int)data, size, false, (int)&msg_id);

    MESSAGE response;
    Call(SYSCALL_GET_MESSAGE_RESPONSE, msg_id, (int)&response);
    return response;
}

void send_message_response(int msg_id, int type, void* data, int size)
{
    Call(SYSCALL_SEND_MESSAGE_RESPONSE, msg_id, type, (int)data, size);
}