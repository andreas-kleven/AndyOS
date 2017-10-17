#include "FileSystem.h"
#include "iso.h"

STATUS FileSystem::ReadFile(char* name, char*& buffer)
{
	return ISO_FS::ReadFile(name, buffer);
}
