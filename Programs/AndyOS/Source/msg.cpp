#include <AndyOS.h>
#include <andyos/msg.h>
#include <unistd.h>

static MESSAGE(*_msg_handler)(MESSAGE);

static void __msg_handler(int id, int src_proc, int type, char* data, int size)
{
    MESSAGE msg(type, data, size);
    msg.id = id;
    msg.src_proc = src_proc;

    MESSAGE response = _msg_handler(msg);

    if (response.type != 0)
        syscall4(SYSCALL_SEND_MESSAGE_RESPONSE, id, response.type, (int)response.data, response.size);
        
    exit_thread(0);
}

int set_message(MESSAGE(*handler)(MESSAGE))
{
    _msg_handler = handler;
    return syscall1(SYSCALL_SET_MESSAGE, (int)__msg_handler);
}

void post_message(int proc_id, int type, void* data, int size)
{
    //Asynchronous
    int msg_id;
    syscall6(SYSCALL_SEND_MESSAGE, proc_id, type, (int)data, size, true, (int)&msg_id);
}

MESSAGE send_message(int proc_id, int type, void* data, int size)
{
    //Synchronous
    int msg_id;
    syscall6(SYSCALL_SEND_MESSAGE, proc_id, type, (int)data, size, false, (int)&msg_id);

    MESSAGE response;
    syscall2(SYSCALL_GET_MESSAGE_RESPONSE, msg_id, (int)&response);

    return response;
}