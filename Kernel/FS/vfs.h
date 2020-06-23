#pragma once
#include "types.h"
#include "Process/scheduler.h"
#include "file.h"
#include "stdio.h"

namespace VFS
{
	int DuplicateFile(int oldfd);
	int DuplicateFile(int oldfd, int newfd);
	
	int Open(const char* filename);
	int Close(int fd);
	size_t Read(int fd, char* buf, size_t size);
	size_t Write(int fd, const char* buf, size_t size);
	off_t Seek(int fd, off_t offset, int whence);
	int CreatePipes(int pipefd[2], int flags);

	uint32 ReadFile(const char* filename, char*& buffer);
	STATUS Init();
};