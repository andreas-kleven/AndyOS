#include "ata.h"
#include "math.h"
#include "HAL/hal.h"
#include "../FS/iso.h"
#include "string.h"
#include "Lib/debug.h"

ATADevice::ATADevice(int bus, int drive)
{
	this->bus = bus;
	this->drive = drive;

	int num = ((bus == ATA_BUS_SECONDARY) << 1) | (drive == ATA_DRIVE_SLAVE);
	char* id = "hd_";
	id[2] = 'a' + num;
	strcpy(this->id, id);

	this->status = DEVICE_STATUS_RUNNING;
}

bool ATADevice::Read(int location, char*& buffer, int length)
{
	if (length <= 0)
		return STATUS_FAILED;

	int size = ATA_SECTOR_SIZE;
	int sectors = (length - 1) / ATA_SECTOR_SIZE + 1;
	buffer = new char[length];

	for (int i = 0; i < sectors; i++)
	{
		if (i == sectors - 1)
			size = length % ATA_SECTOR_SIZE;

		if (!ReadSector(location + i * ATA_SECTOR_SIZE, size, &*buffer + i * ATA_SECTOR_SIZE))
			return STATUS_FAILED;
	}

	return STATUS_SUCCESS;
}

IFileSystem* ATADevice::Mount()
{
	return new ISO_FS(this);
}

inline void Delay(int bus)
{
	inb(bus + ATA_LBA_STATUS);
	inb(bus + ATA_LBA_STATUS);
	inb(bus + ATA_LBA_STATUS);
	inb(bus + ATA_LBA_STATUS);
}

bool ATADevice::ReadSector(int location, int size, char* buffer)
{
	int sector = location / ATA_SECTOR_SIZE;
	int offset = location % ATA_SECTOR_SIZE;

	uint8 read_cmd[12] = { 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	outb(bus + ATA_DRIVE_SELECT, drive & (1 << 4));
	//ATA_SELECT_DELAY(0x1F0);
	Delay(bus);
	outb(bus + ATA_FEATURES, 0);

	outb(bus + ATA_LBA_MID, (ATA_SECTOR_SIZE & 0xFF));
	outb(bus + ATA_LBA_HIGH, (ATA_SECTOR_SIZE >> 8));
	outb(bus + ATA_LBA_COMMAND, 0xA0);

	while (inb(bus + ATA_LBA_STATUS) & 0x80) asm volatile("pause");

	read_cmd[9] = 1;
	read_cmd[2] = (uint8)(sector >> 24);
	read_cmd[3] = (uint8)(sector >> 16);
	read_cmd[4] = (uint8)(sector >> 8);
	read_cmd[5] = (uint8)(sector >> 0);

	for (int i = 0; i < 12; i += 2)
		outw(bus + ATA_DATA, (uint16)((read_cmd[i + 1] << 8) | read_cmd[i]));

	while (inb(bus + ATA_LBA_STATUS) & 0x80) asm volatile("pause");

	int read_size = (inb(bus + ATA_LBA_HIGH) << 8) | inb(bus + ATA_LBA_MID);

	for (int i = 0; i < read_size; i += 2)
	{
		uint16 w = inw(bus + ATA_DATA);
		buffer[i] = w & 0xFF;
		buffer[i + 1] = w >> 8;
	}

	while (inb(bus + ATA_LBA_STATUS) & 0x88) asm volatile("pause");

	return STATUS_SUCCESS;
}

STATUS ATADevice::Init()
{
	IDT::InstallIRQ(0x2E, (IRQ_HANDLER)ATA_Interrupt);
	IDT::InstallIRQ(0x2F, (IRQ_HANDLER)ATA_Interrupt);

	DeviceManager::AddDevice(new ATADevice(ATA_BUS_PRIMARY, ATA_DRIVE_MASTER));
	DeviceManager::AddDevice(new ATADevice(ATA_BUS_PRIMARY, ATA_DRIVE_SLAVE));
	return STATUS_SUCCESS;
}

void ATADevice::ATA_Interrupt(REGS* regs)
{
}