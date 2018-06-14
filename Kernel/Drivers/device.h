#pragma once
#include "definitions.h"

class IFileSystem;

enum DEVICE_TYPE
{
	DEVICE_TYPE_UNKNOWN,
	DEVICE_TYPE_BLOCK
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
	char id[16];
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

class DeviceManager
{
public:
	static Device* first_device;

	static STATUS Init();
	static void AddDevice(Device* device);
};