#include "device.h"
#include "ata.h"

Device* DeviceManager::first_device = 0;

STATUS DeviceManager::Init()
{
	ATADevice::Init();

	return STATUS_SUCCESS;
}

void DeviceManager::AddDevice(Device* device)
{
	if (device->status == DEVICE_STATUS_ERROR)
		return;

	if (first_device)
		device->next = first_device;
	else
		device->next = 0;

	first_device = device;
}
