#pragma once
#include "types.h"
#include "PCI/pci.h"
#include "hal.h"

struct AC97_BUFFER_ENTRY
{
	uint8* buffer;
	uint16 length;
	uint32 reserved : 14;
	uint32 bup : 1;
	uint32 ioc : 1;
} __attribute__((packed));

struct AC97_DEVICE
{
	PciDevice* pci_device;
	uint16 nambar;
	uint16 nabmbar;
	uint8 irq;
	uint8 bits;
	uint8 lvi;
	AC97_BUFFER_ENTRY* bdl;
	uint8** buffers;
} __attribute__((packed));

namespace AC97
{
	STATUS Init(PciDevice* dev);
};