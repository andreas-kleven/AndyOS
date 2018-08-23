#pragma once
#include "types.h"
#include "FS/vfs.h"

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

class Driver : public FileIO
{
public:
	char* name;
	char* id;
	DRIVER_TYPE type;
	DRIVER_STATUS status;
	Driver* next;

	Driver()
	{
		this->status = DRIVER_STATUS_ERROR;
	}
};

class BlockDriver : public Driver
{
public:
	virtual int Read(fpos_t pos, char* buf, size_t size) { return -1; }
	virtual int Write(fpos_t pos, const char* buf, size_t size) { return -1; }

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
	void AddDriver(Driver* driver);
	Driver* GetDriver();
	Driver* GetDriver(const char* id);
	STATUS Init();
};