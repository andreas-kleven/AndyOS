#include "unistd.h"

extern int pipe(int pipefd[2]);

int pipe(int pipefd[2])
{
	return pipe(pipefd);
}