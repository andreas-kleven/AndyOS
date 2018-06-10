#include "vfs.h"
#include "string.h"
#include "path.h"
#include "iso.h"
#include "debug.h"

IFileSystem* VFS::first_fs;
IFileSystem* primary_fs;

STATUS VFS::Init()
{
	Device* dev = DeviceManager::first_device;

	while (dev)
	{
		if (dev->type == DEVICE_TYPE_BLOCK)
		{
			IFileSystem* fs = ((BlockDevice*)dev)->Mount();
			AddDrive(fs);
		}

		dev = dev->next;
	}

	primary_fs = GetDrive("hda");

	return STATUS_SUCCESS;
}

uint32 VFS::ReadFile(char* path, char*& buffer)
{
	char letter = Path::GetDriveLetter(path);

	if (!letter)
		return 0;

	IFileSystem* fs = primary_fs;
	FILE_INFO file;

	if (!fs) return 0;
	if (!fs->GetFile(0, path + 3, &file)) return 0;

	if (fs->ReadFile(&file, buffer))
		return file.size;

	return 0;
}

IFileSystem* VFS::GetDrive(char* id)
{
	IFileSystem* fs = first_fs;

	while (fs)
	{
		if (strcmp(fs->device->id, id) == 0)
		{
			return fs;
		}

		fs = fs->next;
	}
}

void VFS::AddDrive(IFileSystem* fs)
{
	if (fs)
	{
		if (first_fs)
			fs->next = first_fs;
		else
			fs->next = 0;

		first_fs = fs;
	}
}