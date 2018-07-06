#include "device.h"
#include "ata.h"
#include "mouse.h"
#include "string.h"

namespace DeviceManager
{
	Device* first_device = 0;

	void AddDevice(Device* device)
	{
		if (!device)
			return;

		if (device->status == DEVICE_STATUS_ERROR)
			return;

		if (first_device)
			device->next = first_device;
		else
			device->next = 0;

		first_device = device;
	}

	Device* GetDevice()
	{
		return first_device;
	}

	Device* GetDevice(const char* id)
	{
		Device* dev = first_device;

		while (dev)
		{
			if (strcmp(dev->id, id) == 0)
				return dev;

			dev = dev->next;
		}

		return dev;
	}
	
	STATUS Init()
	{
		ATADevice::Init();
		Mouse::Init();

		return STATUS_SUCCESS;
	}
}