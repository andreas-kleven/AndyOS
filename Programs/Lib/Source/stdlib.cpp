#include "stdlib.h"
#include "unistd.h"

void exit(int status)
{
    return _exit(status);
}