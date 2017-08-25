#include "FileSystem.h"
#include "iso.h"

STATUS FileSystem::ReadFile(char* name, char*& buffer)
{
	ISO_FS::ReadFile(name, buffer);
	return STATUS_SUCCESS;
}
