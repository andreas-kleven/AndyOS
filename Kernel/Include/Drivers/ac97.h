#pragma once
#include <hal.h>
#include <pci.h>
#include <types.h>

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

struct AC97_DEVICE
{
    PciDevice *pci_device;
    uint16 nambar;
    uint16 nabmbar;
    uint8 irq;
    uint8 bits;
    uint8 lvi;
    AC97_BUFFER_ENTRY *bdl;
    uint16 **buffers;
    size_t position;
} __attribute__((packed));

namespace AC97 {
STATUS Init(PciDevice *pci_dev);
void Play(void *samples, size_t count);
}; // namespace AC97
