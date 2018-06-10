#include "ata.h"
#include "math.h"
#include "HAL/hal.h"
#include "debug.h"

STATUS ATA::Init()
{
	IDT::InstallIRQ(0x2E, (IRQ_HANDLER)ATA_Interrupt);
	IDT::InstallIRQ(0x2F, (IRQ_HANDLER)ATA_Interrupt);
	return STATUS_SUCCESS;
}

STATUS ATA::Read(int bus, int drive, int sector, char*& buffer, int length)
{
	if (length <= 0)
		return STATUS_FAILED;

	int sectors = (length - 1) / ATA_SECTOR_SIZE + 1;
	buffer = new char[length];

	for (int i = 0; i < sectors; i++)
	{
		if (!ReadSector(bus, drive, sector + i, &*buffer + i * ATA_SECTOR_SIZE))
			return STATUS_FAILED;
	}

	return STATUS_SUCCESS;
}

inline void Delay(int bus)
{
	inb(bus + ATA_LBA_STATUS);
	inb(bus + ATA_LBA_STATUS);
	inb(bus + ATA_LBA_STATUS);
	inb(bus + ATA_LBA_STATUS);
}

STATUS ATA::ReadSector(int bus, int drive, int sector, char* buffer)
{
	uint8 read_cmd[12] = { 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	outb(bus + ATA_DRIVE_SELECT, drive & (1 << 4));
	//ATA_SELECT_DELAY(0x1F0);
	Delay(ATA_BUS_PRIMARY);
	outb(bus + ATA_FEATURES, 0);

	outb(bus + ATA_LBA_MID, (ATA_SECTOR_SIZE & 0xFF));
	outb(bus + ATA_LBA_HIGH, (ATA_SECTOR_SIZE >> 8));
	outb(bus + ATA_LBA_COMMAND, 0xA0);

	while (inb(bus + ATA_LBA_STATUS) & 0x80)  _asm pause;

	read_cmd[9] = 1;
	read_cmd[2] = (uint8)(sector >> 24);
	read_cmd[3] = (uint8)(sector >> 16);
	read_cmd[4] = (uint8)(sector >> 8);
	read_cmd[5] = (uint8)(sector >> 0);

	for (int i = 0; i < 12; i += 2)
		outw(bus + ATA_DATA, (uint16)((read_cmd[i + 1] << 8) | read_cmd[i]));

	while (inb(bus + ATA_LBA_STATUS) & 0x80) _asm pause;

	int size = (inb(bus + ATA_LBA_HIGH) << 8) | inb(bus + ATA_LBA_MID);

	for (int i = 0; i < size; i += 2)
	{
		uint16 w = inw(bus + ATA_DATA);
		buffer[i] = w & 0xFF;
		buffer[i + 1] = w >> 8;

		//if (w == 0xFFFF)
		//{
		//	Debug::Print("%x", sector);
		//	while (1);
		//}
	}

	while (inb(bus + ATA_LBA_STATUS) & 0x88) _asm pause;

	return STATUS_SUCCESS;
}

void ATA::ATA_Interrupt(REGS* regs)
{
}
