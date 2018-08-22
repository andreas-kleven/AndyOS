#include "vfs.h"
#include "filesystem.h"
#include "pipe.h"
#include "string.h"
#include "iso.h"
#include "Lib/debug.h"
#include "Process/process.h"
#include "Process/scheduler.h"
#include "Drivers/driver.h"

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

	FileSystem* GetFS(const Path* path)
	{
		return primary_fs;
	}

	FNODE* GetNode(const Path* path)
	{
		if (path->count == 0)
			return 0;

		for (int i = 0; i < NODE_COUNT; i++)
		{
			FNODE* node = nodes[i];

			if (node)
			{
				if (*node->path == *path)
				{
					return node;
				}
			}
		}

		FileSystem* fs = GetFS(path);
		FNODE* node = new FNODE;

		if (fs)
		{
			if (fs->GetFile(path, node))
			{
				node->io = fs;
			}
			else if (path->count >= 2)
			{
				if (strcmp(path->parts[0], "dev") == 0)
				{
					Driver* drv = DriverManager::GetDriver(path->parts[1]);

					if (drv && drv->type == DRIVER_TYPE_CHAR)
					{
						node->io = drv;
						node->type = FILE_TYPE_CHAR;
					}
				}
				else if (strcmp(path->parts[0], "proc") == 0)
				{
					if (path->count == 4)
					{
						pid_t pid = atoi(path->parts[1]);
						int fd = atoi(path->parts[3]);

						if (pid != 0 && strcmp(path->parts[2], "fd") == 0)
						{
							node->type = FILE_TYPE_CHAR;
						}
					}
				}
			}
		}

		if (!node->io)
		{
			delete node;
			return 0;
		}

		node->parent = GetNode(path->Parent());

		if (node->parent)
		{
			node->next = node->parent->first_child;
			node->parent->first_child = node;
		}

		node->path = (Path*)path;

		if (!AddNode(node))
		{
			delete node;
			return 0;
		}

		return node;
	}

	int AddFile(FILE* file)
	{
		PROCESS* proc = Scheduler::CurrentThread()->process;

		//Find first empty
		for (int i = 3; i < FILE_TABLE_SIZE; i++)
		{
			if (proc->file_table[i] == 0)
			{
				proc->file_table[i] = file;
				return i;
			}
		}

		return -1;
	}

	FILE* GetFile(int fd)
	{
		PROCESS* proc = Scheduler::CurrentThread()->process;

		if (fd >= FILE_TABLE_SIZE)
			return 0;

		return proc->file_table[fd];
	}

	bool SetFile(int fd, FILE* file)
	{
		PROCESS* proc = Scheduler::CurrentThread()->process;

		if (fd >= FILE_TABLE_SIZE)
			return false;

		proc->file_table[fd] = file;
		return true;
	}

	int Open(const char* filename)
	{
		Path* path = new Path(filename);
		FNODE* node = GetNode(path);

		if (!node)
			return -1;

		THREAD* thread = Scheduler::CurrentThread();
		FILE* file = new FILE(node, thread);
		FileIO* io = file->node->io;

		if (!io)
			return -1;

		if (io->Open(file->node, file) != 0)
			return -1;

		return AddFile(file);
	}

	int Close(int fd)
	{
		FILE* file = GetFile(fd);
		delete file;

		if (!file)
			return -1;

		SetFile(fd, 0);
		return 0;
	}

	size_t Read(int fd, char* dst, size_t size)
	{
		FILE* file = GetFile(fd);

		if (!file)
			return 0;

		FNODE* node = file->node;

		int read = node->io->Read(file, dst, size);
		file->pos += read;
		return read;
	}

	size_t Write(int fd, const char* buf, size_t size)
	{
		FILE* file = GetFile(fd);

		if (!file)
			return 0;

		FNODE* node = file->node;

		int written = node->io->Write(file, buf, size);
		file->pos += written;
		return written;
	}

	int Seek(int fd, long int offset, int origin)
	{
		FILE* file = GetFile(fd);

		if (!file)
			return -1;

		switch (origin)
		{
		case SEEK_SET:
			file->pos = offset;
			return 0;

		case SEEK_CUR:
			file->pos += offset;
			return 0;

		case SEEK_END:
			file->pos = file->node->size + offset;
			return 0;

		default:
			return -1;
		}
	}

	int CreatePipes(int pipefd[2], int flags)
	{
		THREAD* thread = Scheduler::CurrentThread();

		Pipe* pipe = new Pipe();
		FNODE* node = new FNODE("/pipes/1", FILE_TYPE_PIPE, pipe);
		
		if (!AddNode(node))
			return -1;

		FILE* read = new FILE(node, thread);
		FILE* write = new FILE(node, thread);

		pipefd[0] = AddFile(read);
		pipefd[1] = AddFile(write);

		if (pipefd[0] == -1 || pipefd[1] == -1)
			return -1;

		return 0;
	}

	uint32 ReadFile(const char* filename, char*& buffer)
	{
		Path* path = new Path(filename);

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