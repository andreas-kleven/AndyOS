#pragma once
#include "types.h"
#include "FS/file.h"

class FileSystem;

enum DRIVER_TYPE
{
	DRIVER_TYPE_BLOCK,
	DRIVER_TYPE_CHAR,
	DRIVER_TYPE_FS
};

enum DRIVER_STATUS
{
	DRIVER_STATUS_ERROR,
	DRIVER_STATUS_RUNNING,
	DRIVER_STATUS_STOPPED
};

class FileIO
{
public:
	virtual int Open(FILE *file) { return 0; }
	virtual int Close(FILE *file) { return 0; }
	virtual int Read(FILE *file, void *buf, size_t size) { return 0; }
	virtual int Write(FILE *file, const void *buf, size_t size) { return 0; }
	virtual int Seek(FILE *file, long offset, int origin) { return 0; }
	virtual int GetChildren(DENTRY *parent, const char *find_name) { return 0; }
};

class Driver : public FileIO
{
public:
	int id;
	const char *name;
	DRIVER_TYPE type;
	DRIVER_STATUS status;
	Driver *next;

	Driver()
	{
		this->status = DRIVER_STATUS_ERROR;
	}
};

class BlockDriver : public Driver
{
public:
	virtual int Read(fpos_t pos, void *buf, size_t size) { return -1; }
	virtual int Write(fpos_t pos, const void *buf, size_t size) { return -1; }

	BlockDriver()
	{
		this->type = DRIVER_TYPE_BLOCK;
	}
};

class CharDriver : public Driver
{
public:
	CharDriver()
	{
		this->type = DRIVER_TYPE_CHAR;
	}
};

namespace DriverManager
{
	void AddDriver(Driver *driver);
	Driver *GetDriver();
	Driver *GetDriver(const char *name);
	STATUS Init();
}; // namespace DriverManager
