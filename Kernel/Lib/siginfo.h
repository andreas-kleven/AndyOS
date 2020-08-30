#pragma once
#include <types.h>

#define SIGINFO_ANY(siginfo) (siginfo.si_code != 0)

#define CLD_EXITED    1
#define CLD_KILLED    2
#define CLD_DUMPED    3
#define CLD_TRAPPED   4
#define CLD_STOPPED   5
#define CLD_CONTINUED 6

typedef struct
{
    int si_status = 0;
    int si_code = 0;
    pid_t si_pid = 0;
} siginfo_t;
