#pragma once
#include "definitions.h"
#include "filesystem.h"
#include "stdio.h"

struct FILE
{
};

namespace VFS
{
	int Open(const char* filename);
	int Close(int fd);
	size_t Read(int fd, char* buf, size_t size);
	size_t Write(int fd, const char* buf, size_t size);
	int Seek(int fd, long int offset, int origin);

	uint32 ReadFile(const char* path, char*& buffer);
	bool List(const char* path, FILE_INFO*& files, DIRECTORY_INFO*& dirs, int& file_count, int& dir_count);
	IFileSystem* GetDrive(const char* id);
	STATUS Init();
};