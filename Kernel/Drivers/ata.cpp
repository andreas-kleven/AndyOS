#include <Drivers/ata.h>
#include <Kernel/timer.h>
#include <io.h>
#include <irq.h>
#include <hal.h>
#include <FS/iso.h>
#include <string.h>
#include <math.h>
#include <debug.h>
#include <Process/scheduler.h>

#define PRD_NUM_ENTRIES 1
#define PRD_BYTES 65536
#define MAX_BYTES (PRD_NUM_ENTRIES * PRD_BYTES)

#define REG_COMMAND 0
#define REG_STATUS 2
#define REG_ADDRESS 4

#define DMA_STOP 0
#define DMA_START (1 << 0)
#define DMA_READ (1 << 3)
#define STATUS_ERR (1 << 1)

struct PRD_ENTRY
{
	uint32 address;
	uint16 bytes;
	int reserved : 15;
	int eot : 1;
} __attribute__((packed));

Mutex mutex1 = Mutex();
Mutex mutex2 = Mutex();
bool received_irq1 = false;
bool received_irq2 = false;
int irq_io_base = 0;

ATADriver::ATADriver(int bus, int drive, PciDevice *pci_dev)
{
	this->bus = bus;
	this->drive = drive;
	this->pci_dev = pci_dev;
	this->is_atapi = false;

	int num = ((bus == ATA_BUS_SECONDARY) << 1) | (drive == ATA_DRIVE_SLAVE);
	char *id = new char[4]{'h', 'd', ' ', 0};
	id[2] = 'a' + num;
	this->name = id;

	this->io_base = pci_dev->config.bar4 & 0xFFFFFFFC;
	irq_io_base = this->io_base;

	if (bus == ATA_BUS_SECONDARY)
		this->io_base += 8;

	if (!Identify(false))
	{
		if (!is_atapi || !Identify(true))
			return;
	}

	pci_dev->EnableBusMastering();
	InitPRDT();

	/*//int read = ReadSectors(2, 1);
	int read = Read(1024, _data_buf, 256);
	debug_print("%d\n", read);
	debug_dump((void *)((size_t)_data_buf), 512, true);
	sys_halt();*/

	this->status = DRIVER_STATUS_RUNNING;
}

int ATADriver::Read(fpos_t pos, void *buf, size_t length)
{
	MutexAquire();

	fpos_t first_sector = pos / block_size;
	fpos_t offset = pos % block_size;
	int parts = DIV_CEIL(offset + length, MAX_BYTES);
	int read = 0;

	for (int i = 0; i < parts; i++)
	{
		int size = min((int)(length - read), MAX_BYTES);
		int count = DIV_CEIL(size, block_size);

		if (ReadSectors(first_sector + i * (MAX_BYTES / block_size), count))
		{
			MutexRelease();
			return -1;
		}

		void *src = (void *)((size_t)data_buffer + offset);
		void *dst = (void *)((size_t)buf + i * MAX_BYTES);
		memcpy(dst, src, size);
		read += size;
	}

	MutexRelease();
	return read;
}

void ATADriver::Delay()
{
	inb(bus + ATA_STATUS);
	inb(bus + ATA_STATUS);
	inb(bus + ATA_STATUS);
	inb(bus + ATA_STATUS);
}

void ATADriver::MutexAquire()
{
	if (bus == ATA_BUS_PRIMARY)
		mutex1.Aquire();
	else
		mutex2.Aquire();
}

void ATADriver::MutexRelease()
{
	if (bus == ATA_BUS_PRIMARY)
		mutex1.Release();
	else
		mutex2.Release();
}

void ATADriver::IrqWaitReset()
{
	if (bus == ATA_BUS_PRIMARY)
		received_irq1 = false;
	else
		received_irq2 = false;
}

void ATADriver::IrqWait()
{
	if (bus == ATA_BUS_PRIMARY)
	{
		while (!received_irq1)
			pause();
	}
	else
	{
		while (!received_irq2)
			pause();
	}
}

bool ATADriver::Identify(bool atapi)
{
	outb(bus + ATA_DRIVE_SELECT, drive);
	Delay();

	outb(bus + ATA_COUNT, 0);
	outb(bus + ATA_LOW, 0);
	outb(bus + ATA_MID, 0);
	outb(bus + ATA_HIGH, 0);

	outb(bus + ATA_COMMAND, atapi ? ATA_CMD_IDENTIFY_PACKET : ATA_CMD_IDENTIFY);
	Delay();

	int status;

	if ((status = inb(bus + ATA_STATUS)) == 0)
	{
		debug_print("Not present\n");
		return false;
	}

	while ((status = inb(bus + ATA_STATUS)) & ATA_STATUS_BSY)
	{
		if (inb(bus + ATA_MID) || inb(bus + ATA_HIGH))
			break;

		pause();
	}

	while (!((status = inb(bus + ATA_STATUS)) & (ATA_STATUS_DRQ | ATA_STATUS_ERR)))
	{
		if (inb(bus + ATA_MID))
			pause();
	}

	if (status & ATA_STATUS_ERR)
	{
		int mid = inb(bus + ATA_MID);
		int high = inb(bus + ATA_HIGH);

		if (mid || high)
		{
			if (mid == 0x14 && high == 0xEB)
			{
				debug_print("Detected ATAPI\n");
				is_atapi = true;
				return false;
			}
			else
			{
				debug_print("Not ATA drive\n");
				return false;
			}
		}
		else
		{
			debug_print("ATA err\n");
			return false;
		}
	}

	this->block_size = is_atapi ? ATAPI_SECTOR_SIZE : ATA_SECTOR_SIZE;

	char buf[512];
	uint16 *ptr = (uint16 *)buf;

	for (int i = 0; i < 256; i++)
	{
		uint16 w = inw(bus);
		ptr[i] = (w >> 8) | (w << 8);
	}

	//debug_dump(buf, sizeof(buf));
	return true;
}

void ATADriver::InitPRDT()
{
	int blocks = BYTES_TO_BLOCKS(sizeof(PRD_ENTRY) * PRD_NUM_ENTRIES);
	void *table = VMem::KernelAlloc(blocks);
	data_buffer = VMem::KernelAlloc(BYTES_TO_BLOCKS(PRD_BYTES * PRD_NUM_ENTRIES), true);

	memset(table, 0, sizeof(PRD_ENTRY) * PRD_NUM_ENTRIES);
	PRD_ENTRY *entries = (PRD_ENTRY *)table;

	for (int i = 0; i < PRD_NUM_ENTRIES; i++)
	{
		entries[i].address = VMem::GetAddress((size_t)data_buffer + i * PRD_BYTES);
		entries[i].bytes = 0;
	}

	entries[PRD_NUM_ENTRIES - 1].eot = 1;

	outl(io_base + REG_ADDRESS, VMem::GetAddress((size_t)table));
}

int ATADriver::ReadSectors(int sector, int count)
{
	IrqWaitReset();

	outb(io_base + REG_COMMAND, DMA_READ);

	outb(bus + ATA_DRIVE_SELECT, drive | 0x40 | ((sector >> 24) & 0x0F));
	Delay();

	outb(bus + ATA_FEATURES, 1);

	if (is_atapi)
	{
		outb(bus + ATA_MID, 0);
		outb(bus + ATA_HIGH, 0);
		outb(bus + ATA_COMMAND, ATA_CMD_PACKET);
	}
	else
	{
		outb(bus + ATA_COUNT, count);
		outb(bus + ATA_LOW, (uint8)(sector & 0xFF));
		outb(bus + ATA_MID, (uint8)(sector >> 8));
		outb(bus + ATA_HIGH, (uint8)(sector >> 16));
		outb(bus + ATA_COMMAND, ATA_CMD_READ_DMA);
	}

	while (inb(bus + ATA_STATUS) & ATA_STATUS_BSY)
		pause();

	if (is_atapi)
	{
		uint8 cmd[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		cmd[9] = count;
		cmd[2] = (uint8)(sector >> 24);
		cmd[3] = (uint8)(sector >> 16);
		cmd[4] = (uint8)(sector >> 8);
		cmd[5] = (uint8)(sector >> 0);

		for (int i = 0; i < 12; i += 2)
			outw(bus + ATA_DATA, (uint16)((cmd[i + 1] << 8) | cmd[i]));
	}

	outb(io_base + REG_COMMAND, DMA_READ | DMA_START);
	IrqWait();
	inb(io_base + REG_STATUS);
	outb(io_base + REG_COMMAND, DMA_STOP);

	return 0;
}

STATUS ATADriver::Init()
{
	IRQ::Install(0x2E, ATA_Interrupt1);
	IRQ::Install(0x2F, ATA_Interrupt2);

	PciDevice *dev = PCI::GetDevice(1, 1, -1);

	if (!dev)
	{
		debug_print("Could not find IDE controller\n");
		return STATUS_FAILED;
	}

	if (dev->config.progIf != 0x80 && dev->config.progIf != 0x8A)
	{
		debug_print("Unknown IDE device\n");
		return STATUS_FAILED;
	}

	DriverManager::AddDriver(new ATADriver(ATA_BUS_PRIMARY, ATA_DRIVE_MASTER, dev));
	DriverManager::AddDriver(new ATADriver(ATA_BUS_PRIMARY, ATA_DRIVE_SLAVE, dev));
	DriverManager::AddDriver(new ATADriver(ATA_BUS_SECONDARY, ATA_DRIVE_MASTER, dev));
	DriverManager::AddDriver(new ATADriver(ATA_BUS_SECONDARY, ATA_DRIVE_SLAVE, dev));

	return STATUS_SUCCESS;
}

void ATADriver::ATA_Interrupt(int bus)
{
	int io_base = bus == ATA_BUS_PRIMARY ? irq_io_base : (irq_io_base + 8);
	inb(io_base + REG_STATUS);
	outb(io_base + REG_STATUS, 4);

	if (inb(bus + ATA_STATUS) & 1)
		debug_print("ATA error %p\n", inb(bus + ATA_STATUS_ERR));
}

void ATADriver::ATA_Interrupt1()
{
	ATA_Interrupt(ATA_BUS_PRIMARY);
	received_irq1 = true;
}

void ATADriver::ATA_Interrupt2()
{
	ATA_Interrupt(ATA_BUS_SECONDARY);
	received_irq2 = true;
}
