#pragma once
#include "definitions.h"
#include "Process/scheduler.h"
#include "file.h"
#include "stdio.h"

namespace VFS
{
	int Open(const char* filename);
	int Close(int fd);
	size_t Read(int fd, char* buf, size_t size);
	size_t Write(int fd, const char* buf, size_t size);
	int Seek(int fd, long offset, int origin);
	int CreatePipes(int pipefd[2], int flags);

	uint32 ReadFile(const char* filename, char*& buffer);
	STATUS Init();
};