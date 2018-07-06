#include "vfs.h"
#include "string.h"
#include "path.h"
#include "iso.h"
#include "Lib/debug.h"
#include "Process/process.h"
#include "Process/scheduler.h"
#include "Drivers/driver.h"
#include "filesystem.h"

#define NODE_COUNT 256

namespace VFS
{
	FileSystem* first_fs;
	FileSystem* primary_fs;

	FNODE* nodes[NODE_COUNT];

	int Mount(BlockDriver* driver, char* mount_point)
	{
		FileSystem* fs = new ISO_FS(driver);
		fs->mount_point = mount_point;

		first_fs = fs;
		primary_fs = fs;
	}

	bool AddNode(FNODE* node)
	{
		for (int i = 0; i < NODE_COUNT; i++)
		{
			if (nodes[i] == 0)
			{
				nodes[i] = node;
				return true;
			}
		}

		return false;
	}

	FileSystem* GetFS(const char* path)
	{
		return primary_fs;
	}

	FNODE* GetNode(const char* path)
	{
		for (int i = 0; i < NODE_COUNT; i++)
		{
			FNODE* node = nodes[i];

			if (node)
			{
				if (strcmp(node->path, path) == 0)
				{
					return node;
				}
			}
		}

		FileSystem* fs = GetFS(path);
		FNODE* node = new FNODE;

		if (fs)
		{
			if (fs->GetFile(path, node) == 0)
			{
				node->io = fs;
			}
			else
			{
				const char* id = path + 5;
				Driver* drv = DriverManager::GetDriver(id);

				if (drv && drv->type == DRIVER_TYPE_CHAR)
				{
					node->io = drv;
					node->type = FILE_TYPE_CHAR;
				}
				else
				{
					delete node;
					return 0;
				}
			}
		}

		node->path = new char[strlen(path) + 1];
		strcpy(node->path, path);
		AddNode(node);

		return node;
	}

	int Open(const char* filename)
	{
		FILE* file = new FILE;
		file->thread = Scheduler::CurrentThread();
		file->node = GetNode(filename);

		if (!file->node)
			return 0;

		FileIO* io = file->node->io;

		if (io->Open(file->node, file) != SUCCESS)
		{
			return 0;
		}

		return (int)file;
	}

	int Close(int fd)
	{
		if (!fd)
			return -1;

		FILE* file = (FILE*)fd;
		delete file;
		return 1;
	}

	size_t Read(int fd, char* dst, size_t size)
	{
		if (!fd)
			return 0;

		FILE* file = (FILE*)fd;
		FNODE* node = file->node;

		int read = node->io->Read(file, dst, size);
		file->pos += read;
		return read;
	}

	size_t Write(int fd, const char* buf, size_t size)
	{
		if (!fd)
			return 0;
	}

	int Seek(int fd, long int offset, int origin)
	{
		if (!fd)
			return 0;

		FILE* file = (FILE*)fd;

		switch (origin)
		{
		case SEEK_SET:
			file->pos = offset;
			return 1;

		case SEEK_CUR:
			file->pos += offset;
			return 1;

		case SEEK_END:
			file->pos = file->node->size + offset;
			return 1;

		default:
			return 0;
		}
	}

	uint32 ReadFile(const char* path, char*& buffer)
	{
		FILE file;
		file.node = GetNode(path);
		file.thread = Scheduler::CurrentThread();

		if (!file.node)
			return 0;

		buffer = new char[file.node->size];
		int length = file.node->io->Read(&file, buffer, file.node->size);

		return length;
	}

	STATUS Init()
	{
		memset(nodes, 0, sizeof(FNODE*) * NODE_COUNT);

		first_fs = 0;
		primary_fs = 0;

		Driver* drv = DriverManager::GetDriver();

		while (drv)
		{
			if (drv->type == DRIVER_TYPE_BLOCK)
			{
				Mount((BlockDriver*)drv, "/");
				return STATUS_SUCCESS;
			}

			drv = drv->next;
		}

		return STATUS_SUCCESS;
	}
}