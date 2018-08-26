#pragma once
#include "syscall_list.h"

#define syscall0(id)                                syscall6(id, 0, 0, 0, 0, 0, 0)
#define syscall1(id, arg0)                          syscall6(id, arg0, 0, 0, 0, 0, 0)
#define syscall2(id, arg0, arg1)                    syscall6(id, arg0, arg1, 0, 0, 0, 0)
#define syscall3(id, arg0, arg1, arg2)              syscall6(id, arg0, arg1, arg2, 0, 0, 0)
#define syscall4(id, arg0, arg1, arg2, arg3)        syscall6(id, arg0, arg1, arg2, arg3, 0, 0)
#define syscall5(id, arg0, arg1, arg2, arg3, arg4)  syscall6(id, arg0, arg1, arg2, arg3, arg4, 0)


#ifdef __cplusplus
extern "C" {
#endif

int syscall6(int id, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5);

#ifdef __cplusplus
}
#endif