#pragma once
#include <driver.h>
#include <hal.h>
#include <pci.h>
#include <types.h>

#define AC97_IOCTL_SET_VOLUME 0x4101

#define AC97_BDL_LEN     32      /* Buffer descriptor list length */
#define AC97_BDL_SAMPLES 0x10000 /* Length of buffer in BDL */

struct AC97_BUFFER_ENTRY
{
    uint16 *buffer;
    uint16 length;
    uint32 reserved : 14;
    uint32 bup : 1;
    uint32 ioc : 1;
} __attribute__((packed));

class AC97Driver : public CharDriver
{
  public:
    PciDevice *pci_device;
    uint16 nambar;
    uint16 nabmbar;
    uint8 irq;
    uint8 bits;
    uint8 lvi;
    AC97_BUFFER_ENTRY *bdl;
    uint16 **buffers;
    uint8 position;
    Event ready_event;

    AC97Driver(PciDevice *pci_dev);

    int Write(FILE *file, const void *buf, size_t size);
    int Ioctl(FILE *file, int request, unsigned int arg);

    static void Init();

  private:
    int WriteBuffer(void *samples, size_t count);
    void SetVolume(int volume);
    void SetSampleRate();

    static void AC97_Interrupt();
};
