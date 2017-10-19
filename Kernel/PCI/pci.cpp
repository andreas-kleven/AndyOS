#include "PCI/pci.h"
#include "HAL/hal.h"
#include "debug.h"

void printdev(PCI_DEVICE* dev)
{
	Debug::Print("Bus: %x Device: %x Function: %x Vendor: %x Device: %x Class: %x Subclass: %x\n",
		dev->bus, dev->device, dev->func, dev->configSpace.vendorID, dev->configSpace.deviceID,
		dev->configSpace.classCode, dev->configSpace.subclass);
}

STATUS PCI::Init()
{


	return STATUS_SUCCESS;
}

PCI_DEVICE* PCI::GetDevice(int classCode, int subClass, int progIF)
{
	for (uint8 bus = 0; bus < 255; bus++)
	{
		for (uint8 device = 0; device < 32; device++)
		{
			for (uint8 func = 0; func < 8; func++)
			{
				uint32 vendor_id = Read(bus, device, func, 0, 2);

				if (vendor_id != 0xFFFF)
				{
					PCI_DEVICE* dev = Create(bus, device, func);
					printdev(dev);

					if (dev->configSpace.classCode == classCode &&
						dev->configSpace.subclass == subClass &&
						dev->configSpace.progIF == progIF)
					{
						printdev(dev);
						//Debug::Dump(&dev->configSpace, 64);
						return dev;
					}
				}
			}
		}
	}

	return 0;
}

uint32 PCI::Read(uint8 bus, uint8 device, uint8 func, uint8 port, uint8 len)
{
	uint32 val = 0x80000000 | (bus << 16) | (device << 11) | (func << 8) | (port & 0xFC);
	outl(0x0CF8, val);

	uint32 ret = inl(0xCFC) >> port * 8;
	ret &= (0xFFFFFFFF >> ((4 - len) * 8));

	return ret;
}

PCI_DEVICE* PCI::Create(uint8 bus, uint8 device, uint8 func)
{
	PCI_DEVICE* dev = new PCI_DEVICE;
	PCI_CONFIG_SPACE config;

	char buffer[64];

	for (int i = 0; i < 64; i++)
		buffer[i] = Read(bus, device, func, i, 1);

	config = *(PCI_CONFIG_SPACE*)buffer;

	dev->bus = bus;
	dev->device = device;
	dev->func = func;
	dev->configSpace = config;
	return dev;
}