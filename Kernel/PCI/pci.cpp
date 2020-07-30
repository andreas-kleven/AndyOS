#include <pci.h>
#include <hal.h>
#include <debug.h>

PciDevice::PciDevice(int bus, int device, int func)
{
	this->bus = bus;
	this->device = device;
	this->func = func;
	this->id = PCI_ID(bus, device, func);

	ReadConfig();
}

void PciDevice::ReadConfig()
{
	uint16 *buffer = (uint16 *)&config;

	for (int i = 0; i < 32; i++)
		buffer[i] = PCI::Read16(id, i * 2);
}

void PciDevice::EnableBusMastering()
{
	PCI::Write16(id, PCI_CONFIG_COMMAND, config.command | 4);
}

namespace PCI
{
	void printdev(PciDevice *dev)
	{
		debug_print("Bus: %x Device: %x Function: %x Vendor: %x Device: %x Class: %x Subclass: %x ProgIf: %x\n",
					dev->bus, dev->device, dev->func, dev->config.vendor, dev->config.device,
					dev->config.classCode, dev->config.subclass, dev->config.progIf);
	}

	uint16 Read16(uint32 id, uint8 port)
	{
		uint32 addr = 0x80000000 | id | (port & 0xFC);
		outl(PCI_CONFIG_ADDR, addr);
		return inw(PCI_CONFIG_DATA + (port & 2));
	}

	void Write16(uint32 id, uint8 port, uint16 val)
	{
		uint32 addr = 0x80000000 | id | (port & 0xFC);
		outl(PCI_CONFIG_ADDR, addr);
		outw(PCI_CONFIG_DATA + (port & 2), val);
	}

	PciDevice *GetDevice(int classCode, int subClass, int progIf)
	{
		for (uint8 bus = 0; bus < 255; bus++)
		{
			for (uint8 device = 0; device < 32; device++)
			{
				for (uint8 func = 0; func < 8; func++)
				{
					uint32 id = PCI_ID(bus, device, func);
					uint16 vendor_id = Read16(id, PCI_CONFIG_VENDOR_ID);

					if (vendor_id != 0xFFFF)
					{
						PciDevice *dev = new PciDevice(bus, device, func);
						printdev(dev);

						if ((dev->config.classCode == classCode || classCode == -1) &&
							(dev->config.subclass == subClass || subClass == -1) &&
							(dev->config.progIf == progIf || progIf == -1))
						{
							return dev;
						}
						else
						{
							delete dev;
						}
					}
				}
			}
		}

		return 0;
	}

	STATUS Init()
	{
		return STATUS_SUCCESS;
	}
} // namespace PCI
