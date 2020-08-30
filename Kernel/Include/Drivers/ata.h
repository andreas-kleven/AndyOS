#pragma once
#include <driver.h>
#include <pci.h>
#include <types.h>

#define ATA_SECTOR_SIZE   512
#define ATAPI_SECTOR_SIZE 2048

#define ATA_BUS_PRIMARY   0x1F0
#define ATA_BUS_SECONDARY 0x170
#define ATA_DRIVE_MASTER  0xA0
#define ATA_DRIVE_SLAVE   0xB0

#define ATA_DATA           0x00
#define ATA_FEATURES       0x01
#define ATA_COUNT          0x02
#define ATA_LOW            0x03
#define ATA_MID            0x04
#define ATA_HIGH           0x05
#define ATA_DRIVE          0x06
#define ATA_DRIVE_SELECT   6
#define ATA_COMMAND        0x07
#define ATA_STATUS         0x07
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D

#define ATA_STATUS_BSY  0x80
#define ATA_STATUS_DRDY 0x40
#define ATA_STATUS_DRQ  0x08
#define ATA_STATUS_ERR  0x01
#define ATA_STATUS_DF   0x20

#define ATA_CMD_READ_PIO        0x20
#define ATA_CMD_READ_PIO_EXT    0x24
#define ATA_CMD_READ_DMA        0xC8
#define ATA_CMD_READ_DMA_EXT    0x25
#define ATA_CMD_WRITE_PIO       0x30
#define ATA_CMD_WRITE_PIO_EXT   0x34
#define ATA_CMD_WRITE_DMA       0xCA
#define ATA_CMD_WRITE_DMA_EXT   0x35
#define ATA_CMD_CACHE_FLUSH     0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET          0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY        0xEC

class ATADriver : public BlockDriver
{
  private:
    int bus;
    int drive;
    PciDevice *pci_dev;
    size_t io_base;
    int block_size;
    bool is_atapi;
    void *data_buffer;

  public:
    ATADriver(int bus, int drive, PciDevice *pci_dev);
    int Read(fpos_t pos, void *buf, size_t size);
    static STATUS Init();

  private:
    void Delay();
    void MutexAquire();
    void MutexRelease();
    void IrqWaitReset();
    void IrqWait();
    bool Identify(bool atapi);
    void InitPRDT();
    int ReadSectors(int sector, int count);
    static void ATA_Interrupt(int bus);
    static void ATA_Interrupt1();
    static void ATA_Interrupt2();
};
