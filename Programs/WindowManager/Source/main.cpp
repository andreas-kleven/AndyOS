#include <AndyOS.h>
#include "manager.h"
#include "stdio.h"
#include "stdlib.h"

int main()
{
	Drawing::Init();
	WindowManager::Start();
	exit(0);
}