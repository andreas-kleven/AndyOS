#pragma once

#include <sys/types.h>

#define WNOHANG 1
#define WUNTRACED 2

#define WIFEXITED(status) (!WTERMSIG(status))
#define WEXITSTATUS(status) (((status) >> 8) & 0xff)
#define WIFSIGNALED(status) (!WIFSTOPPED(status) && !WIFEXITED(status))
#define WTERMSIG(status) ((status)&0x7f)
#define WIFSTOPPED(status) (((status)&0xff) == 0x7f)
#define WSTOPSIG(status) WEXITSTATUS(status)
#define WCOREDUMP(status) ((status)&0x80)

struct rusage;

/*typedef enum
{
    P_ALL,
    P_PID,
    P_PGID
} idtype_t;*/

pid_t _wait(int *);
pid_t wait(int *);
pid_t wait3(int *status, int options, struct rusage *rusage);
pid_t wait4(pid_t pid, int *status, int options, struct rusage *rusage);
pid_t waitpid(pid_t, int *, int);
//pid_t waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);
