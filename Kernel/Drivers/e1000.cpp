#include "e1000.h"
#include "HAL/hal.h"
#include "Net/net.h"
#include "Net/eth.h"
#include "Memory/memory.h"
#include "Lib/debug.h"

#define REG_CTRL 		0x0000
#define REG_EEPROM 		0x0014
#define REG_IMASK 		0x00D0
#define REG_RCTRL 		0x0100
#define REG_RXDESCLO  	0x2800
#define REG_RXDESCHI  	0x2804
#define REG_RXDESCLEN 	0x2808
#define REG_RXDESCHEAD 	0x2810
#define REG_RXDESCTAIL 	0x2818

#define REG_TCTRL 		0x0400
#define REG_TXDESCLO  	0x3800
#define REG_TXDESCHI  	0x3804
#define REG_TXDESCLEN 	0x3808
#define REG_TXDESCHEAD 	0x3810
#define REG_TXDESCTAIL 	0x3818

#define RCTL_EN					(1 << 1)    // Receiver Enable
#define RCTL_SBP				(1 << 2)    // Store Bad Packets
#define RCTL_UPE				(1 << 3)    // Unicast Promiscuous Enabled
#define RCTL_MPE				(1 << 4)    // Multicast Promiscuous Enabled
#define RCTL_LPE				(1 << 5)    // Long Packet Reception Enable
#define RCTL_LBM_NONE			(0 << 6)    // No Loopback
#define RCTL_LBM_PHY			(3 << 6)    // PHY or external SerDesc loopback
#define RTCL_RDMTS_HALF			(0 << 8)    // Free Buffer Threshold is 1/2 of RDLEN
#define RTCL_RDMTS_QUARTER		(1 << 8)    // Free Buffer Threshold is 1/4 of RDLEN
#define RTCL_RDMTS_EIGHTH		(2 << 8)    // Free Buffer Threshold is 1/8 of RDLEN
#define RCTL_MO_36				(0 << 12)   // Multicast Offset - bits 47:36
#define RCTL_MO_35				(1 << 12)   // Multicast Offset - bits 46:35
#define RCTL_MO_34				(2 << 12)   // Multicast Offset - bits 45:34
#define RCTL_MO_32				(3 << 12)   // Multicast Offset - bits 43:32
#define RCTL_BAM				(1 << 15)   // Broadcast Accept Mode
#define RCTL_VFE				(1 << 18)   // VLAN Filter Enable
#define RCTL_CFIEN				(1 << 19)   // Canonical Form Indicator Enable
#define RCTL_CFI				(1 << 20)   // Canonical Form Indicator Bit Value
#define RCTL_DPF				(1 << 22)   // Discard Pause Frames
#define RCTL_PMCF				(1 << 23)   // Pass MAC Control Frames
#define RCTL_SECRC				(1 << 26)   // Strip Ethernet CRC

#define RCTL_BSIZE_256			(3 << 16)
#define RCTL_BSIZE_512			(2 << 16)
#define RCTL_BSIZE_1024			(1 << 16)
#define RCTL_BSIZE_2048			(0 << 16)
#define RCTL_BSIZE_4096			((3 << 16) | (1 << 25))
#define RCTL_BSIZE_8192			((2 << 16) | (1 << 25))
#define RCTL_BSIZE_16384		((1 << 16) | (1 << 25))

#define ECTRL_FD   	0x01 //FULL DUPLEX
#define ECTRL_ASDE 	0x20 //auto speed enable
#define ECTRL_SLU  	0x40 //set link up

E1000* instance;

E1000::E1000(PciDevice* pci_dev) : NetInterface(pci_dev)
{
	instance = this;

	irq = dev->config.interruptLine;
	dev->EnableBusMastering();

	uint32 mmio_base = dev->config.bar0;
	mem_base = (uint32)VMem::KernelMapFirstFree(mmio_base, PTE_PRESENT | PTE_WRITABLE, 6);

	DetectEEPROM();
	ReadMac();

	uint32 addr = htonl(0xC0A80001);
	gateway_addr = *(IPv4Address*)&addr;

	Net::PrintIP("IP: ", GetIP());
	Net::PrintMac("Mac: ", GetMac());
	Net::PrintIP("Gateway IP: ", gateway_addr);

	Start();
}

void E1000::Send(NetPacket* pkt)
{
	uint32 phys = VMem::GetAddress((uint32)pkt->start);

	tx_descs[tx_cur]->addr = (uint64)phys;
	tx_descs[tx_cur]->length = pkt->length;
	tx_descs[tx_cur]->cmd = ((1 << 3) | 3);

	uint8 old_cur = tx_cur;

	tx_cur = (tx_cur + 1) % E1000_NUM_TX_DESC;
	WriteCommand(REG_TXDESCTAIL, tx_cur);

	while (!(tx_descs[old_cur]->status & 0xff));
}

MacAddress E1000::GetMac()
{
	return *(MacAddress*)mac;
}

IPv4Address E1000::GetIP()
{
	//uint32 ipa = htonl(0xC0A8387E); //192.168.56.126
	uint32 ipa = htonl(0xC0A8007E); //192.168.0.126
	//uint32 ipa = htonl(0x0A000001); //10.0.0.1
	IPv4Address addr = *(IPv4Address*)&ipa;
	return addr;
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
	
	while (rx_descs[rx_cur]->status & 0x1)
	{
		uint8* buf = (uint8*)rx_virt_addr[rx_cur];
		uint16 len = rx_descs[rx_cur]->length;
		uint8* end = buf + len;

		NetPacket pkt;
		pkt.start = buf;
		pkt.end = end;
		pkt.length = len;

		Eth::Receive(this, &pkt);

		rx_descs[rx_cur]->status = 0;
		old_cur = rx_cur;
		rx_cur = (rx_cur + 1) % E1000_NUM_RX_DESC;
		WriteCommand(REG_RXDESCTAIL, old_cur);
	}
}

void E1000::EnableIRQ()
{
	IDT::InstallIRQ(0x20 + irq, (IRQ_HANDLER)E1000_Interrupt);

	WriteCommand(REG_IMASK, 0x1F6DC);
	WriteCommand(REG_IMASK, 0xff & ~4);
	ReadCommand(0xc0);
}

void E1000::E1000_Interrupt(REGS* regs)
{
	uint32 status = instance->ReadCommand(0xc0);

	if (status & 0x04)
	{
		instance->Linkup();
	}

	if (status & 0x10) { }

	if (status & 0x80)
	{
		instance->Poll();
	}
}


void E1000::InitRX()
{
	int blocks = BYTES_TO_BLOCKS(sizeof(E1000_RX_DESC) * E1000_NUM_RX_DESC + 16);
	void* ptr = VMem::KernelAlloc(blocks);

	E1000_RX_DESC* descs = (E1000_RX_DESC*)ptr;
	for (int i = 0; i < E1000_NUM_RX_DESC; i++)
	{
		uint32 addr = (uint32)VMem::KernelAlloc(BYTES_TO_BLOCKS(8192 + 16));
		rx_virt_addr[i] = addr;

		rx_descs[i] = &descs[i];
		rx_descs[i]->addr = (uint64)VMem::GetAddress(addr);
		rx_descs[i]->status = 0;
	}

	WriteCommand(REG_RXDESCLO, VMem::GetAddress((uint32)ptr));
	WriteCommand(REG_RXDESCHI, 0);

	WriteCommand(REG_RXDESCLEN, E1000_NUM_RX_DESC * sizeof(E1000_RX_DESC));

	WriteCommand(REG_RXDESCHEAD, 0);
	WriteCommand(REG_RXDESCTAIL, E1000_NUM_RX_DESC - 1);
	rx_cur = 0;

	uint32 flags = RCTL_EN | RCTL_SBP | RCTL_UPE | RCTL_MPE | RCTL_LBM_NONE | RTCL_RDMTS_HALF | RCTL_BAM | RCTL_SECRC | RCTL_BSIZE_8192;
	WriteCommand(REG_RCTRL, flags);
}

void E1000::InitTX()
{
	int blocks = BYTES_TO_BLOCKS(sizeof(E1000_TX_DESC) * E1000_NUM_TX_DESC + 16);
	void* ptr = VMem::KernelAlloc(blocks);

	E1000_TX_DESC* descs = (E1000_TX_DESC*)ptr;
	for (int i = 0; i < E1000_NUM_TX_DESC; i++)
	{
		tx_descs[i] = &descs[i];
		tx_descs[i]->addr = 0;
		tx_descs[i]->cmd = 0;
		tx_descs[i]->status = 0;
	}

	WriteCommand(REG_TXDESCLO, VMem::GetAddress((uint32)ptr));
	WriteCommand(REG_TXDESCHI, 0);

	WriteCommand(REG_TXDESCLEN, E1000_NUM_TX_DESC * sizeof(E1000_TX_DESC));

	WriteCommand(REG_TXDESCHEAD, 0);
	WriteCommand(REG_TXDESCTAIL, E1000_NUM_TX_DESC - 1);
	tx_cur = 0;

	WriteCommand(REG_TCTRL, (1 << 1) | (1 << 3));
}

bool E1000::DetectEEPROM()
{
	uint32 val = 0;
	WriteCommand(REG_EEPROM, 0x1);

	for (int i = 0; i < 1000 && !eeprom_exists; i++)
	{
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

	if (eeprom_exists)
	{
		WriteCommand(REG_EEPROM, (1) | ((uint32)(addr) << 8));
		while (!((tmp = ReadCommand(REG_EEPROM)) & (1 << 4)));
	}
	else
	{
		WriteCommand(REG_EEPROM, (1) | ((uint32)(addr) << 2));
		while (!((tmp = ReadCommand(REG_EEPROM)) & (1 << 1)));
	}

	data = (uint16)((tmp >> 16) & 0xFFFF);
	return data;
}

void E1000::WriteCommand(uint16 addr, uint32 val)
{
	if (true)
	{
		mmio_write32(mem_base + addr, val);
	}
	else
	{
		outw(io_base, addr);
		outl(io_base + 4, val);
	}
}

uint32 E1000::ReadCommand(uint16 addr)
{
	if (true)
    {
		return mmio_read16(mem_base + addr);
    }
    else
    {
		outw(io_base, addr);
		return inl(io_base + 4);
    }
}

void E1000::ReadMac()
{
	if (false && eeprom_exists)
	{
		uint32 temp;
		temp = ReadEEPROM(0);
		mac[0] = temp & 0xff;
		mac[1] = temp >> 8;
		temp = ReadEEPROM(1);
		mac[2] = temp & 0xff;
		mac[3] = temp >> 8;
		temp = ReadEEPROM(2);
		mac[4] = temp & 0xff;
		mac[5] = temp >> 8;
	}
	else
	{
		uint8* mac8 = (uint8*)(mem_base + 0x5400);
		uint32* mac32 = (uint32*)(mem_base + 0x5400);

		if (mac32[0] != 0)
		{
			for (int i = 0; i < 6; i++)
			{
				mac[i] = mac8[i];
			}
		}
	}
}