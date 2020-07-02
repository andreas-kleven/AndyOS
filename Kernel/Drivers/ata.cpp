#include "ata.h"
#include "io.h"
#include "irq.h"
#include "hal.h"
#include "FS/iso.h"
#include "string.h"
#include "math.h"
#include "Lib/debug.h"
#include "Process/scheduler.h"

ATADriver::ATADriver(int bus, int drive)
{
	this->bus = bus;
	this->drive = drive;

	int num = ((bus == ATA_BUS_SECONDARY) << 1) | (drive == ATA_DRIVE_SLAVE);

	char *id = new char[4]{'h', 'd', ' ', 0};
	id[2] = 'a' + num;
	this->name = id;

	this->status = DRIVER_STATUS_RUNNING;
}

int ATADriver::Read(fpos_t pos, void *buf, size_t length)
{
	if (length <= 0)
		return -1;

	Scheduler::Disable();

	int offset = (int)pos % ATA_SECTOR_SIZE;
	int sectors = DIV_CEIL(offset + length, ATA_SECTOR_SIZE);
	int read = 0;

	for (int i = 0; i < sectors; i++)
	{
		int size = min((int)(length - read), ATA_SECTOR_SIZE);

		int r = ReadSector(pos + i * ATA_SECTOR_SIZE, (uint8 *)buf + i * ATA_SECTOR_SIZE, size);

		if (!r)
		{
			Scheduler::Enable();
			return -1;
		}

		read += r;
	}

	Scheduler::Enable();
	return read;
}

inline void Delay(int bus)
{
	inb(bus + ATA_LBA_STATUS);
	inb(bus + ATA_LBA_STATUS);
	inb(bus + ATA_LBA_STATUS);
	inb(bus + ATA_LBA_STATUS);
}

int ATADriver::ReadSector(fpos_t pos, uint8 *buf, size_t size)
{
	fpos_t sector = pos / ATA_SECTOR_SIZE;
	fpos_t offset = pos % ATA_SECTOR_SIZE;

	uint8 read_cmd[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	outb(bus + ATA_DRIVE_SELECT, drive & (1 << 4));
	//ATA_SELECT_DELAY(0x1F0);
	Delay(bus);
	outb(bus + ATA_FEATURES, 0);

	outb(bus + ATA_LBA_MID, (ATA_SECTOR_SIZE & 0xFF));
	outb(bus + ATA_LBA_HIGH, (ATA_SECTOR_SIZE >> 8));
	outb(bus + ATA_LBA_COMMAND, 0xA0);

	while (inb(bus + ATA_LBA_STATUS) & 0x80)
		pause();

	read_cmd[9] = 1;
	read_cmd[2] = (uint8)(sector >> 24);
	read_cmd[3] = (uint8)(sector >> 16);
	read_cmd[4] = (uint8)(sector >> 8);
	read_cmd[5] = (uint8)(sector >> 0);

	for (int i = 0; i < 12; i += 2)
		outw(bus + ATA_DATA, (uint16)((read_cmd[i + 1] << 8) | read_cmd[i]));

	while (inb(bus + ATA_LBA_STATUS) & 0x80)
		pause();

	size_t read_size = (inb(bus + ATA_LBA_HIGH) << 8) | inb(bus + ATA_LBA_MID);

	for (size_t i = 0; i < read_size; i += 2)
	{
		uint16 w = inw(bus + ATA_DATA);

		if (i >= offset && i < offset + size)
			buf[i - offset] = w & 0xFF;

		if ((i + 1) >= offset && (i + 1) < offset + size)
			buf[i + 1 - offset] = w >> 8;
	}

	while (inb(bus + ATA_LBA_STATUS) & 0x88)
		pause();

	return size;
}

STATUS ATADriver::Init()
{
	IRQ::Install(0x2E, ATA_Interrupt);
	IRQ::Install(0x2F, ATA_Interrupt);

	DriverManager::AddDriver(new ATADriver(ATA_BUS_PRIMARY, ATA_DRIVE_MASTER));
	//DriverManager::AddDriver(new ATADriver(ATA_BUS_PRIMARY, ATA_DRIVE_SLAVE));
	DriverManager::AddDriver(new ATADriver(ATA_BUS_SECONDARY, ATA_DRIVE_MASTER));
	//DriverManager::AddDriver(new ATADriver(ATA_BUS_SECONDARY, ATA_DRIVE_SLAVE));

	return STATUS_SUCCESS;
}

void ATADriver::ATA_Interrupt()
{
}