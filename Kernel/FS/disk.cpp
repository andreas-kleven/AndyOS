#include "disk.h"
#include "string.h"
#include "Memory/memory.h"
#include "Drivers/ata.h"
#include "debug.h"

STATUS Disk::Read(int sector, char*& buffer, int length)
{
	return ATA::Read(ATA_BUS_PRIMARY, ATA_DRIVE_MASTER, sector, buffer, length);
}
