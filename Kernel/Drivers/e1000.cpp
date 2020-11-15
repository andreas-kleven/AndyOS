#include <Drivers/e1000.h>
#include <Kernel/dpc.h>
#include <Net/eth.h>
#include <Net/packetmanager.h>
#include <debug.h>
#include <hal.h>
#include <io.h>
#include <irq.h>
#include <memory.h>
#include <net.h>

#define REG_CTRL       0x0000
#define REG_STATUS     0x0008
#define REG_EEPROM     0x0014
#define REG_CTRL_EXT   0x0018
#define REG_IMASK      0x00D0
#define REG_RCTRL      0x0100
#define REG_RXDESCLO   0x2800
#define REG_RXDESCHI   0x2804
#define REG_RXDESCLEN  0x2808
#define REG_RXDESCHEAD 0x2810
#define REG_RXDESCTAIL 0x2818

#define REG_TCTRL      0x0400
#define REG_TXDESCLO   0x3800
#define REG_TXDESCHI   0x3804
#define REG_TXDESCLEN  0x3808
#define REG_TXDESCHEAD 0x3810
#define REG_TXDESCTAIL 0x3818

#define REG_RDTR   0x2820 // RX Delay Timer Register
#define REG_RXDCTL 0x3828 // RX Descriptor Control
#define REG_RADV   0x282C // RX Int. Absolute Delay Timer
#define REG_RSRPD  0x2C00 // RX Small Packet Detect Interrupt

#define REG_TIPG  0x0410 // Transmit Inter Packet Gap
#define ECTRL_SLU 0x40   // set link up

#define RCTL_EN            (1 << 1)  // Receiver Enable
#define RCTL_SBP           (1 << 2)  // Store Bad Packets
#define RCTL_UPE           (1 << 3)  // Unicast Promiscuous Enabled
#define RCTL_MPE           (1 << 4)  // Multicast Promiscuous Enabled
#define RCTL_LPE           (1 << 5)  // Long Packet Reception Enable
#define RCTL_LBM_NONE      (0 << 6)  // No Loopback
#define RCTL_LBM_PHY       (3 << 6)  // PHY or external SerDesc loopback
#define RTCL_RDMTS_HALF    (0 << 8)  // Free Buffer Threshold is 1/2 of RDLEN
#define RTCL_RDMTS_QUARTER (1 << 8)  // Free Buffer Threshold is 1/4 of RDLEN
#define RTCL_RDMTS_EIGHTH  (2 << 8)  // Free Buffer Threshold is 1/8 of RDLEN
#define RCTL_MO_36         (0 << 12) // Multicast Offset - bits 47:36
#define RCTL_MO_35         (1 << 12) // Multicast Offset - bits 46:35
#define RCTL_MO_34         (2 << 12) // Multicast Offset - bits 45:34
#define RCTL_MO_32         (3 << 12) // Multicast Offset - bits 43:32
#define RCTL_BAM           (1 << 15) // Broadcast Accept Mode
#define RCTL_VFE           (1 << 18) // VLAN Filter Enable
#define RCTL_CFIEN         (1 << 19) // Canonical Form Indicator Enable
#define RCTL_CFI           (1 << 20) // Canonical Form Indicator Bit Value
#define RCTL_DPF           (1 << 22) // Discard Pause Frames
#define RCTL_PMCF          (1 << 23) // Pass MAC Control Frames
#define RCTL_SECRC         (1 << 26) // Strip Ethernet CRC

// Buffer Sizes
#define RCTL_BSIZE_256   (3 << 16)
#define RCTL_BSIZE_512   (2 << 16)
#define RCTL_BSIZE_1024  (1 << 16)
#define RCTL_BSIZE_2048  (0 << 16)
#define RCTL_BSIZE_4096  ((3 << 16) | (1 << 25))
#define RCTL_BSIZE_8192  ((2 << 16) | (1 << 25))
#define RCTL_BSIZE_16384 ((1 << 16) | (1 << 25))

// Transmit Command
#define CMD_EOP  (1 << 0) // End of Packet
#define CMD_IFCS (1 << 1) // Insert FCS
#define CMD_IC   (1 << 2) // Insert Checksum
#define CMD_RS   (1 << 3) // Report Status
#define CMD_RPS  (1 << 4) // Report Packet Sent
#define CMD_VLE  (1 << 6) // VLAN Packet Enable
#define CMD_IDE  (1 << 7) // Interrupt Delay Enable

// TCTL Register
#define TCTL_EN         (1 << 1)  // Transmit Enable
#define TCTL_PSP        (1 << 3)  // Pad Short Packets
#define TCTL_CT_SHIFT   4         // Collision Threshold
#define TCTL_COLD_SHIFT 12        // Collision Distance
#define TCTL_SWXOFF     (1 << 22) // Software XOFF Transmission
#define TCTL_RTLC       (1 << 24) // Re-transmit on Late Collision

#define TSTA_DD (1 << 0) // Descriptor Done
#define TSTA_EC (1 << 1) // Excess Collisions
#define TSTA_LC (1 << 2) // Late Collision
#define LSTA_TU (1 << 3) // Transmit Underrun

static E1000 *instance;

E1000::E1000(PciDevice *pci_dev) : NetInterface(pci_dev)
{
    instance = this;

    irq = pci_dev->config.interruptLine;
    pci_dev->EnableBusMastering();

    uint32 mmio_addr = pci_dev->config.bar0 & 0xFFFFFFF0;
    uint32 io_addr = pci_dev->config.bar0 & 0xFFFFFFFC;
    bar_type = (pci_dev->config.bar0 >> 1) & 2;
    mem_base = (size_t)VMem::KernelMapFirstFree((void *)mmio_addr, 6, PAGE_PRESENT | PAGE_WRITE);
    io_base = (size_t)VMem::KernelMapFirstFree((void *)io_addr, 6, PAGE_PRESENT | PAGE_WRITE);

    // DetectEEPROM();
    eeprom_exists = false;

    ReadMac();

    gateway_addr = htonl(0x00000000); // 0.0.0.0

    Net::PrintIP("IP: ", GetIP());
    Net::PrintMac("Mac: ", GetMac());
    Net::PrintIP("Gateway IP: ", gateway_addr);

    Start();
}

void E1000::Send(NetPacket *pkt)
{
    size_t phys = VMem::GetAddress((size_t)pkt->start);

    tx_descs[tx_cur]->addr = (uint64)phys;
    tx_descs[tx_cur]->length = pkt->length;
    tx_descs[tx_cur]->cmd = ((1 << 3) | 3);
    tx_descs[tx_cur]->status = 0;

    uint8 old_cur = tx_cur;

    tx_cur = (tx_cur + 1) % E1000_NUM_TX_DESC;
    WriteCommand(REG_TXDESCTAIL, tx_cur);

    while (!(tx_descs[old_cur]->status & 0xff))
        pause();
}

MacAddress E1000::GetMac()
{
    return mac;
}

uint32 E1000::GetIP()
{
    return htonl(0x0A0000FE); // 10.0.0.254
}

uint32 E1000::GetMask()
{
    return htonl(0xFFFFFF00);
}

void E1000::Start()
{
    Linkup();

    for (int i = 0; i < 0x80; i++)
        WriteCommand(0x5200 + i * 4, 0);

    EnableIRQ();

    InitRX();
    InitTX();

    uint32 flags = ReadCommand(REG_RCTRL);
    WriteCommand(REG_RCTRL, flags | RCTL_EN);
}

void E1000::Linkup()
{
    uint32 val = ReadCommand(REG_CTRL);
    WriteCommand(REG_CTRL, val | ECTRL_SLU);
}

void E1000::Poll()
{
    uint16 old_cur;

    while (rx_descs[rx_cur]->status & 0x1) {
        uint8 *buf = (uint8 *)rx_virt_addr[rx_cur];
        uint16 len = rx_descs[rx_cur]->length;
        uint8 *end = buf + len;

        NetPacket pkt;
        pkt.interface = instance;
        pkt.start = buf;
        pkt.end = end;
        pkt.length = len;

        ETH::HandlePacket(&pkt);

        // TODO: Put in syscall handler?
        rx_descs[rx_cur]->status = 0;
        old_cur = rx_cur;
        rx_cur = (rx_cur + 1) % E1000_NUM_RX_DESC;
        WriteCommand(REG_RXDESCTAIL, old_cur);
    }
}

void E1000::EnableIRQ()
{
    IRQ::Install(0x20 + irq, E1000_Interrupt);

    WriteCommand(REG_IMASK, 0x1F6DC);
    WriteCommand(REG_IMASK, 0xff & ~4);
    ReadCommand(0xc0);
}

void E1000::E1000_Interrupt()
{
    uint32 status = instance->ReadCommand(0xc0);

    if (status & 0x04) {
        instance->Linkup();
    }

    if (status & 0x10) {
    }

    if (status & 0x80) {
        Dpc::Create((DPC_HANDLER)PacketManager::Poll, instance);
    }
}

void E1000::InitRX()
{
    int blocks = BYTES_TO_BLOCKS(sizeof(E1000_RX_DESC) * E1000_NUM_RX_DESC + 16);
    void *ptr = VMem::KernelAlloc(blocks);

    E1000_RX_DESC *descs = (E1000_RX_DESC *)ptr;
    for (int i = 0; i < E1000_NUM_RX_DESC; i++) {
        size_t addr = (size_t)VMem::KernelAlloc(BYTES_TO_BLOCKS(8192 + 16));
        rx_virt_addr[i] = addr;

        rx_descs[i] = &descs[i];
        rx_descs[i]->addr = (uint64)VMem::GetAddress(addr);
        rx_descs[i]->status = 0;
    }

    size_t addr = VMem::GetAddress((size_t)ptr);
    WriteCommand(REG_TXDESCLO, (uint32)((uint64)addr >> 32));
    WriteCommand(REG_TXDESCHI, (uint32)((uint64)addr & 0xFFFFFFFF));
    WriteCommand(REG_RXDESCLO, (uint64)addr);
    WriteCommand(REG_RXDESCHI, 0);
    WriteCommand(REG_RXDESCLEN, E1000_NUM_RX_DESC * sizeof(E1000_RX_DESC));
    WriteCommand(REG_RXDESCHEAD, 0);
    WriteCommand(REG_RXDESCTAIL, E1000_NUM_RX_DESC - 1);

    rx_cur = 0;

    uint32 flags = REG_RCTRL | RCTL_EN | RCTL_SBP | RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE |
                   RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC | RCTL_BSIZE_8192;
    WriteCommand(REG_RCTRL, flags);
}

void E1000::InitTX()
{
    int blocks = BYTES_TO_BLOCKS(sizeof(E1000_TX_DESC) * E1000_NUM_TX_DESC + 16);
    void *ptr = VMem::KernelAlloc(blocks);

    E1000_TX_DESC *descs = (E1000_TX_DESC *)ptr;
    for (int i = 0; i < E1000_NUM_TX_DESC; i++) {
        tx_descs[i] = &descs[i];
        tx_descs[i]->addr = 0;
        tx_descs[i]->cmd = 0;
        tx_descs[i]->status = 0;
    }

    size_t addr = VMem::GetAddress((size_t)ptr);
    WriteCommand(REG_TXDESCHI, (uint32)((uint64)addr >> 32));
    WriteCommand(REG_TXDESCLO, (uint32)((uint64)addr & 0xFFFFFFFF));
    WriteCommand(REG_TXDESCLEN, E1000_NUM_TX_DESC * sizeof(E1000_TX_DESC));
    WriteCommand(REG_TXDESCHEAD, 0);
    WriteCommand(REG_TXDESCTAIL, 0);

    tx_cur = 0;

    uint32 flags = TCTL_EN | TCTL_PSP | (15 << TCTL_CT_SHIFT) | (64 << TCTL_COLD_SHIFT) | TCTL_RTLC;
    WriteCommand(REG_TCTRL, flags);
}

bool E1000::DetectEEPROM()
{
    uint32 val = 0;
    WriteCommand(REG_EEPROM, 0x1);

    for (int i = 0; i < 1000 && !eeprom_exists; i++) {
        val = ReadCommand(REG_EEPROM);
        if (val & 0x10)
            eeprom_exists = true;
        else
            eeprom_exists = false;
    }

    return eeprom_exists;
}

uint32 E1000::ReadEEPROM(uint8 addr)
{
    uint16 data = 0;
    uint32 tmp = 0;

    if (eeprom_exists) {
        WriteCommand(REG_EEPROM, (1) | ((uint32)(addr) << 8));
        while (!((tmp = ReadCommand(REG_EEPROM)) & (1 << 4)))
            ;
    } else {
        WriteCommand(REG_EEPROM, (1) | ((uint32)(addr) << 2));
        while (!((tmp = ReadCommand(REG_EEPROM)) & (1 << 1)))
            ;
    }

    data = (uint16)((tmp >> 16) & 0xFFFF);
    return data;
}

void E1000::WriteCommand(uint16 addr, uint32 val)
{
    if (bar_type == 0) {
        mmio_write32(mem_base + addr, val);
    } else {
        outw(io_base, addr);
        outl(io_base + 4, val);
    }
}

uint32 E1000::ReadCommand(uint16 addr)
{
    if (bar_type == 0) {
        return mmio_read16(mem_base + addr);
    } else {
        outw(io_base, addr);
        return inl(io_base + 4);
    }
}

void E1000::ReadMac()
{
    if (eeprom_exists) {
        uint32 temp;
        temp = ReadEEPROM(0);
        mac.n[0] = temp & 0xff;
        mac.n[1] = temp >> 8;
        temp = ReadEEPROM(1);
        mac.n[2] = temp & 0xff;
        mac.n[3] = temp >> 8;
        temp = ReadEEPROM(2);
        mac.n[4] = temp & 0xff;
        mac.n[5] = temp >> 8;
    } else {
        uint32 p1 = mmio_read32(mem_base + 0x5400);
        uint16 p2 = mmio_read16(mem_base + 0x5400 + 4);

        mac.n[0] = p1 & 0xFF;
        mac.n[1] = p1 >> 8;
        mac.n[2] = p1 >> 16;
        mac.n[3] = p1 >> 24;
        mac.n[4] = p2 & 0xFF;
        mac.n[5] = p2 >> 8;
    }
}
