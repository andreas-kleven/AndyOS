#pragma once
#include <types.h>

#define PCI_CONFIG_ADDR 0xcf8
#define PCI_CONFIG_DATA 0xcfC

// Header Type
#define PCI_TYPE_MULTIFUNC      0x80
#define PCI_TYPE_GENERIC        0x00
#define PCI_TYPE_PCI_BRIDGE     0x01
#define PCI_TYPE_CARDBUS_BRIDGE 0x02

// PCI Configuration Registers
#define PCI_CONFIG_VENDOR_ID      0x00
#define PCI_CONFIG_DEVICE_ID      0x02
#define PCI_CONFIG_COMMAND        0x04
#define PCI_CONFIG_STATUS         0x06
#define PCI_CONFIG_REVISION_ID    0x08
#define PCI_CONFIG_PROG_INTF      0x09
#define PCI_CONFIG_SUBCLASS       0x0a
#define PCI_CONFIG_CLASS_CODE     0x0b
#define PCI_CONFIG_CACHELINE_SIZE 0x0c
#define PCI_CONFIG_LATENCY        0x0d
#define PCI_CONFIG_HEADER_TYPE    0x0e
#define PCI_CONFIG_BIST           0x0f

// Type 0x00 (Generic) Configuration Registers
#define PCI_CONFIG_BAR0                0x10
#define PCI_CONFIG_BAR1                0x14
#define PCI_CONFIG_BAR2                0x18
#define PCI_CONFIG_BAR3                0x1c
#define PCI_CONFIG_BAR4                0x20
#define PCI_CONFIG_BAR5                0x24
#define PCI_CONFIG_CARDBUS_CIS         0x28
#define PCI_CONFIG_SUBSYSTEM_VENDOR_ID 0x2c
#define PCI_CONFIG_SUBSYSTEM_DEVICE_ID 0x2e
#define PCI_CONFIG_EXPANSION_ROM       0x30
#define PCI_CONFIG_CAPABILITIES        0x34
#define PCI_CONFIG_INTERRUPT_LINE      0x3c
#define PCI_CONFIG_INTERRUPT_PIN       0x3d
#define PCI_CONFIG_MIN_GRANT           0x3e
#define PCI_CONFIG_MAX_LATENCY         0x3f

// ------------------------------------------------------------------------------------------------
// PCI BAR

#define PCI_BAR_IO       0x01
#define PCI_BAR_LOWMEM   0x02
#define PCI_BAR_64       0x04
#define PCI_BAR_PREFETCH 0x08

#define PCI_ID(bus, dev, func) (((bus) << 16) | ((device) << 11) | ((func) << 8))

struct PCI_CONFIG_SPACE
{
    uint16 vendor;
    uint16 device;

    uint16 command;
    uint16 status;

    uint8 revision;
    uint8 progIf;
    uint8 subclass;
    uint8 classCode;

    uint8 cacheLineSize;
    uint8 latencyTimer;
    uint8 headerType;
    uint8 bist;

    uint32 bar0;
    uint32 bar1;
    uint32 bar2;
    uint32 bar3;
    uint32 bar4;
    uint32 bar5;

    uint32 cisptr;

    uint16 subsystem;
    uint16 subsystemVendor;

    uint32 expansionROMBaseAddress;

    uint8 capabilitiesPointer;
    uint8 reserved1;
    uint8 reserved2;
    uint8 reserved3;

    uint32 reserved4;

    uint8 interruptLine;
    uint8 interruptPin;
    uint8 minGrant;
    uint8 maxLatency;
} __attribute__((packed));

class PciDevice
{
  public:
    PciDevice(int bus, int device, int func);

    uint8 bus;
    uint8 device;
    uint8 func;
    uint32 id;

    PCI_CONFIG_SPACE config;

    void ReadConfig();
    void EnableBusMastering();
};

namespace PCI {
uint16 Read16(uint32 id, uint8 port);
void Write16(uint32 id, uint8 port, uint16 val);

PciDevice *GetDevice(int classCode, int subClass, int progIf);
STATUS Init();
}; // namespace PCI