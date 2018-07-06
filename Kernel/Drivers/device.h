#pragma once
#include "definitions.h"

class IFileSystem;

enum DEVICE_TYPE
{
	DEVICE_TYPE_UNKNOWN,
	DEVICE_TYPE_BLOCK,
	DEVICE_TYPE_CHAR
};

enum DEVICE_STATUS
{
	DEVICE_STATUS_ERROR,
	DEVICE_STATUS_RUNNING,
	DEVICE_STATUS_STOPPED
};

class Device
{
public:
	char* name;
	char* id;
	DEVICE_TYPE type;
	DEVICE_STATUS status;
	Device* next;

	Device()
	{
		status = DEVICE_STATUS_ERROR;
	}
};

class BlockDevice : public Device
{
public:
	virtual bool Read(int location, char*& buffer, int length) { return 0; }
	virtual bool Write(int location, char*& buffer, int length) { return 0; }
	virtual IFileSystem* Mount() { return 0; }

	BlockDevice()
	{
		type = DEVICE_TYPE_BLOCK;
	}
};

class CharDevice : public Device
{
public:
	virtual int Read(char* buf, size_t size, int pos) { return 0; };
	virtual void Write(char c) { };

	CharDevice()
	{
		type = DEVICE_TYPE_CHAR;
	}
};

namespace DeviceManager
{
	void AddDevice(Device* device);
	Device* GetDevice();
	Device* GetDevice(const char* id);
	STATUS Init();
};