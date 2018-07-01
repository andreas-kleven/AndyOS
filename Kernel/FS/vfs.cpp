#include "vfs.h"
#include "string.h"
#include "path.h"
#include "iso.h"
#include "Lib/debug.h"

namespace VFS
{
	IFileSystem* first_fs;
	IFileSystem* primary_fs;

	void AddDrive(IFileSystem* fs)
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

	uint32 ReadFile(char* path, char*& buffer)
	{
		//char letter = Path::GetDriveLetter(path);
		//
		//if (!letter)
		//	return 0;

		IFileSystem* fs = primary_fs;
		FILE_INFO file;

		if (!fs) return 0;
		if (!fs->GetFile(0, path, &file)) return 0;

		if (fs->ReadFile(&file, buffer))
			return file.size;

		return 0;
	}

	bool List(char* path, FILE_INFO*& files, DIRECTORY_INFO*& dirs, int& file_count, int& dir_count)
	{
		file_count = 0;
		dir_count = 0;

		if (!primary_fs)
			return 0;

		return primary_fs->List(path, files, dirs, file_count, dir_count);
	}

	IFileSystem* GetDrive(char* id)
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

	STATUS Init()
	{
		first_fs = 0;
		primary_fs = 0;

		Device* dev = DeviceManager::GetDevice();

		while (dev)
		{
			if (dev->type == DEVICE_TYPE_BLOCK)
			{
				IFileSystem* fs = ((BlockDevice*)dev)->Mount();
				AddDrive(fs);
			}

			dev = dev->next;
		}

		primary_fs = GetDrive("hdc");

		return STATUS_SUCCESS;
	}
}