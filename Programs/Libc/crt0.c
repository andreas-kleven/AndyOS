#include <stdlib.h>
#include <unistd.h>

extern int main(int argc, char **argv, char **envp);

extern void (**__init_array_start)();
extern void (**__init_array_end)();

void _start(int argc, char **argv, char **envp)
{
    for (void (**p)() = &__init_array_start; p < &__init_array_end; ++p)
        (*p)();

    _init();

    environ = envp;
    int ex = main(argc, argv, envp);

    _fini();
    exit(ex);
}
