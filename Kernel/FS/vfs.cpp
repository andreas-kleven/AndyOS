#include "vfs.h"
#include "string.h"
#include "path.h"
#include "iso.h"
#include "Lib/debug.h"
#include "Process/process.h"
#include "Process/scheduler.h"
#include "Drivers/device.h"

namespace VFS
{
	enum FILE_TYPE
	{
		FILE_TYPE_REGULAR,
		FILE_TYPE_DIRECTORY,
		FILE_TYPE_BLOCK,
		FILE_TYPE_CHAR
	};

	struct FILE_NODE
	{
		FILE_TYPE type;
		char* filename = 0;
		uint32 location = 0;
		uint32 size = 0;
		IFileSystem* fs = 0;
		Device* dev = 0;
	};

	struct FILE_DESC
	{
		FILE_NODE* node;
		size_t pos = 0;
		PROCESS* owner = 0;
		THREAD* thread = 0;
	};

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

	FILE_NODE* GetNode(const char* filename)
	{
		FILE_NODE* node = new FILE_NODE();

		IFileSystem* fs = primary_fs;
		FILE_INFO file;

		if (!fs) return 0;
		if (fs->GetFile(0, filename, &file))
		{
			node->type = FILE_TYPE_REGULAR;
			node->location = file.location;
			node->size = file.size;
			node->fs = fs;

			char* tmp;
			node->size = ReadFile(filename, tmp);
		}
		else
		{
			const char* id = filename + 5;
			node->dev = DeviceManager::GetDevice(id);
			node->type = FILE_TYPE_CHAR;

			if (node->dev == 0)
				return 0;
		}

		node->filename = new char[strlen(filename) + 1];
		strcpy(node->filename, filename);

		return node;
	}

	int Open(const char* filename)
	{
		FILE_DESC* desc = new FILE_DESC;
		desc->node = GetNode(filename);
		desc->owner = Scheduler::CurrentThread()->process;
		return (int)desc;
	}

	int Close(int fd)
	{
		FILE_DESC* desc = (FILE_DESC*)fd;
		delete desc;
		return 1;
	}

	size_t Read(int fd, char* dst, size_t size)
	{
		FILE_DESC* desc = (FILE_DESC*)fd;
		FILE_NODE* node = desc->node;
		char* buf;

		switch (node->type)
		{
		case FILE_TYPE_REGULAR:
		{
			ReadFile(node->filename, buf);
			memcpy(dst, buf + desc->pos, size);
			desc->pos += size;
			return size;
		}

		case FILE_TYPE_CHAR:
		{
			CharDevice* dev = (CharDevice*)node->dev;
			int read = dev->Read(dst, size, desc->pos);
			desc->pos += read;
			return read;
		}

		default:
			return 0;
		}
	}

	size_t Write(int fd, const char* buf, size_t size)
	{

	}

	int Seek(int fd, long int offset, int origin)
	{
		FILE_DESC* desc = (FILE_DESC*)fd;

		switch (origin)
		{
		case SEEK_SET:
			desc->pos = offset;
			return 1;

		case SEEK_CUR:
			desc->pos += offset;
			return 1;

		case SEEK_END:
			desc->pos = desc->node->size + offset;
			return 1;

		default:
			return 0;
		}
	}

	uint32 ReadFile(const char* path, char*& buffer)
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

	IFileSystem* GetDrive(const char* id)
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