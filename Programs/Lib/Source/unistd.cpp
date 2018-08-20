#include "unistd.h"

extern int sys_pipe(int pipefd[2]);

int pipe(int pipefd[2])
{
	return sys_pipe(pipefd);
}