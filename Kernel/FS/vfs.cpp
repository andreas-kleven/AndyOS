#include "vfs.h"
#include "string.h"
#include "path.h"
#include "iso.h"
#include "debug.h"

STATUS VFS::Init()
{
	ISO_FS::Init();
	return STATUS_SUCCESS;
}

uint32 VFS::ReadFile(char* path, char*& buffer)
{
	char letter = Path::GetDriveLetter(path);

	if (!letter)
		return 0;

	IFileSystem* filesys = ISO_FS::instance;
	FILE_INFO file;

	if (!filesys) return 0;
	if (!filesys->GetFile(0, path + 3, &file)) return 0;
	if (file.size == 0) return 0;

	if (filesys->ReadFile(&file, buffer)) 
		return file.size;

	return 0;
}
