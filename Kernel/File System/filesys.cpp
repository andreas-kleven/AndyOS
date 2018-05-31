#include "filesys.h"
#include "iso.h"

STATUS FS::Init()
{
	ISO_FS::Init();
	return STATUS_SUCCESS;
}

uint32 FS::ReadFile(char* name, char*& buffer)
{
	return ISO_FS::ReadFile(name, buffer);
}
