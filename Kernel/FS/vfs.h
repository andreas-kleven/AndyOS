#pragma once
#include "definitions.h"
#include "Process/scheduler.h"
#include "stdio.h"

#define SUCCESS 0
#define ERROR   1

class FileIO;

enum FILE_TYPE
{
	FILE_TYPE_REGULAR,
	FILE_TYPE_DIRECTORY,
	FILE_TYPE_BLOCK,
	FILE_TYPE_CHAR
};

struct FNODE
{
	char* path = 0;
	size_t size = 0;
	long pos = 0;
	FILE_TYPE type;
	FileIO* io;
};

struct FILE
{
	FNODE* node;
	long pos = 0;
	THREAD* thread = 0;
};

class FileIO
{
public:
    virtual int Open(FNODE* node, FILE* file) { return 0; }
    virtual int Close(FILE* file) { return -1; }
    virtual int Read(FILE* file, char* buf, size_t size) { return -1; }
    virtual int Write(FILE* file, const char* buf, size_t size) { return -1; }
    virtual int Seek(FILE* file, long offset, int origin) { return -1; }
};

namespace VFS
{
	int Open(const char* filename);
	int Close(int fd);
	size_t Read(int fd, char* buf, size_t size);
	size_t Write(int fd, const char* buf, size_t size);
	int Seek(int fd, long offset, int origin);

	uint32 ReadFile(const char* path, char*& buffer);
	//bool List(const char* path, FILE_INFO*& files, DIRECTORY_INFO*& dirs, int& file_count, int& dir_count);
	//FileSystem* GetDrive(const char* id);
	STATUS Init();
};