#include <stdlib.h>
#include <unistd.h>

extern int main(int argc, char **argv, char **envp);

void _start(int argc, char **argv, char **envp)
{
    environ = envp;
    exit(main(argc, argv, envp));
}
