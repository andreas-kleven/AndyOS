#include "device.h"
#include "ata.h"

namespace DeviceManager
{
	Device* first_device = 0;

	void AddDevice(Device* device)
	{
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
	
	STATUS Init()
	{
		ATADevice::Init();

		return STATUS_SUCCESS;
	}
}