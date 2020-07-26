#pragma once

#define WNOHANG 1
#define WUNTRACED 2

#define WIFEXITED(status) (!WTERMSIG(status))
#define WEXITSTATUS(status) (((status) >> 8) & 0xff)
#define WIFSIGNALED(status) (!WIFSTOPPED(status) && !WIFEXITED(status))
#define WTERMSIG(status) ((status)&0x7f)
#define WIFSTOPPED(status) (((status)&0xff) == 0x7f)
#define WSTOPSIG(status) WEXITSTATUS(status)
#define WCOREDUMP(status) ((status)&0x80)

typedef enum
{
    P_ALL,
    P_PID,
    P_PGID
} idtype_t;
