#include "syscall_list.h"
#include <pthread.h>

int pthread_stub(void *(*start_routine)(void *), void *arg)
{
    pthread_exit(start_routine(arg));
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *),
                   void *arg)
{
    return syscall5(SYSCALL_THREAD_CREATE, thread, attr, pthread_stub, start_routine, arg);
}

void pthread_exit(void *retval)
{
    syscall1(SYSCALL_THREAD_EXIT, retval);
    return 0;
}
