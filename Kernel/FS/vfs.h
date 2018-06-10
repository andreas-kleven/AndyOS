#pragma once
#include "definitions.h"
#include "filesystem.h"

class VFS
{
public:
	static IFileSystem* first_fs;

	static STATUS Init();
	static uint32 ReadFile(char* path, char*& buffer);
	static IFileSystem* GetDrive(char* id);

private:
	static void AddDrive(IFileSystem* fs);
};