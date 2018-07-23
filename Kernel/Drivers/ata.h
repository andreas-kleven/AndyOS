#pragma once
#include "driver.h"
#include "definitions.h"

#define ATA_SECTOR_SIZE 2048

#define ATA_BUS_PRIMARY 0x1F0
#define ATA_BUS_SECONDARY 0x170
#define ATA_DRIVE_MASTER 0xA0
#define ATA_DRIVE_SLAVE 0xB0

#define ATA_DATA 0
#define ATA_FEATURES 1
#define ATA_COUNT 2
#define ATA_LBA_LOW 3
#define ATA_LBA_MID 4
#define ATA_LBA_HIGH 5
#define ATA_LBA_DRIVE 6
#define ATA_LBA_COMMAND 7
#define ATA_LBA_STATUS 7

#define ATA_DRIVE_SELECT 6

#define ATA_STATUS_BSY  0x80
#define ATA_STATUS_DRDY 0x40
#define ATA_STATUS_DRQ  0x08
#define ATA_STATUS_ERR  0x01
#define ATA_STATUS_DF   0x20

class ATADriver : public BlockDriver
{
public:
	int bus;
	int drive;

	ATADriver(int bus, int drive);

	int Read(long pos, char* buf, size_t size);

	static STATUS Init();

private:
	int ReadSector(long pos, char* buf, size_t size);

	static void ATA_Interrupt();
};