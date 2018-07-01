#pragma once
#include "definitions.h"
#include "filesystem.h"

namespace VFS
{
	uint32 ReadFile(char* path, char*& buffer);
	bool List(char* path, FILE_INFO*& files, DIRECTORY_INFO*& dirs, int& file_count, int& dir_count);
	IFileSystem* GetDrive(char* id);
	STATUS Init();
};