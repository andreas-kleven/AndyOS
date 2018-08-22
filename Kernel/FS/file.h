#pragma once
#include "path.h"

class FileIO;

enum FILE_TYPE
{
	FILE_TYPE_REGULAR,
	FILE_TYPE_DIRECTORY,
	FILE_TYPE_BLOCK,
	FILE_TYPE_CHAR,
    FILE_TYPE_PIPE
};

struct FNODE
{
	Path path;
	size_t size = 0;
	long pos = 0;
	FILE_TYPE type;
	FileIO* io;

	FNODE* parent = 0;
	FNODE* next = 0;
	FNODE* first_child = 0;

    FNODE()
    { }

    FNODE(char* path, FILE_TYPE type, FileIO* io)
    {
        this->path = Path(path);
        this->type = type;
        this->io = io;
    }
};

struct FILE
{
	FNODE* node;
	long pos = 0;
	THREAD* thread = 0;

    FILE()
    { }

    FILE(FNODE* node, THREAD* thread)
    {
        this->node = node;
        this->thread = thread;
    }
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