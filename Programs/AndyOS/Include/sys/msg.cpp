#include "msg.h"
#include <AndyOS.h>
#include "syscall.h"

static void(*_sig_handler)(int);
static MESSAGE(*_msg_handler)(MESSAGE);

static void __sig_handler(int signo)
{
    _sig_handler(signo);
    exit_thread(0);
}

static void __msg_handler(int id, int src_proc, int type, char* data, int size)
{
    MESSAGE msg(type, data, size);
    msg.id = id;
    msg.src_proc = src_proc;

    MESSAGE response = _msg_handler(msg);

    if (response.type != 0)
        syscall(SYSCALL_SEND_MESSAGE_RESPONSE, id, response.type, (int)response.data, response.size);
        
    exit_thread(0);
}


int set_signal(void(*handler)(int))
{
    _sig_handler = handler;
    return syscall(SYSCALL_SET_SIGNAL, (int)__sig_handler);
}

void send_signal(int proc_id, int signo)
{
    syscall(SYSCALL_SEND_SIGNAL, proc_id, signo);
}

int set_message(MESSAGE(*handler)(MESSAGE))
{
    _msg_handler = handler;
    return syscall(SYSCALL_SET_MESSAGE, (int)__msg_handler);
}

void post_message(int proc_id, int type, void* data, int size)
{
    //Asynchronous
    int msg_id;
    syscall(SYSCALL_SEND_MESSAGE, proc_id, type, (int)data, size, true, (int)&msg_id);
}

MESSAGE send_message(int proc_id, int type, void* data, int size)
{
    //Synchronous
    int msg_id;
    syscall(SYSCALL_SEND_MESSAGE, proc_id, type, (int)data, size, false, (int)&msg_id);

    MESSAGE response;
    syscall(SYSCALL_GET_MESSAGE_RESPONSE, msg_id, (int)&response);

    return response;
}