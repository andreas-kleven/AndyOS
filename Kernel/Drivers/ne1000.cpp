#include "ne1000.h"
#include "HAL/hal.h"
#include "Net/net.h"
#include "Net/eth.h"
#include "debug.h"

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

#define RCTRL_EN 	0x00000002
#define RCTRL_SBP 	0x00000004
#define RCTRL_UPE 	0x00000008
#define RCTRL_MPE 	0x00000010
#define RCTRL_8192 	0x00030000

#define ECTRL_FD   	0x01//FULL DUPLEX
#define ECTRL_ASDE 	0x20//auto speed enable
#define ECTRL_SLU  	0x40//set link up

E1000* E1000::global_e;

E1000::E1000(PCI_DEVICE* pci_dev) : NetInterface(pci_dev)
{
	global_e = this;

	io_base = dev->configSpace.BAR2 & ~1;
	mem_base = dev->configSpace.BAR2 & ~3;
	irq = dev->configSpace.interruptLine;

	DetectEEPROM();
	ReadMac();

	Net::PrintIP("IP: ", GetIP());
	Net::PrintMac("Mac: ", GetMac());

	Start();
}

void E1000::Send(NetPacket* pkt)
{
	//Debug::Dump(pkt->start, pkt->length);

	tx_descs[tx_cur]->addr = (uint64)pkt->start;
	tx_descs[tx_cur]->length = pkt->length;
	tx_descs[tx_cur]->cmd = ((1 << 3) | 3);

	uint8 old_cur = tx_cur;

	tx_cur = (tx_cur + 1) % NUM_TX_DESC;
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
	//uint32 ipa = htonl(0xC0A8007E); //192.168.0.126
	IPv4Address addr = Net::GatewayIPv4;
	addr.n[3] = 126;

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
	WriteCommand(REG_RCTRL, flags | RCTRL_EN);
}

void E1000::Linkup()
{
	uint32 val;
	val = ReadCommand(REG_CTRL);
	WriteCommand(REG_CTRL, val | ECTRL_SLU);
}

void E1000::Poll()
{
	uint16 old_cur;

	while (rx_descs[rx_cur]->status & 0x1)
	{
		uint8* buf = (uint8*)rx_descs[rx_cur]->addr;
		uint16 len = rx_descs[rx_cur]->length;
		uint8* end = buf + len;

		NetPacket pkt;
		pkt.start = buf;
		pkt.end = end;
		pkt.length = len;

		Eth::Receive(this, &pkt);

		rx_descs[rx_cur]->status = 0;
		old_cur = rx_cur;
		rx_cur = (rx_cur + 1) % NUM_RX_DESC;
		WriteCommand(REG_RXDESCTAIL, old_cur);
	}
}


void E1000::EnableIRQ()
{
	IDT::SetISR(0x20 + irq, (IRQ_HANDLER)E1000_Interrupt);

	WriteCommand(REG_IMASK, 0x1F6DC);
	WriteCommand(REG_IMASK, 0xff & ~4);
	ReadCommand(0xc0);
}

void INTERRUPT E1000::E1000_Interrupt()
{
	_asm pushad

	if (1)
	{
		uint32 status = global_e->ReadCommand(0xc0);

		if (status & 0x04)
		{
			global_e->Linkup();
		}

		if (status & 0x10) {}

		if (status & 0x80)
		{
			global_e->Poll();
		}

		//Debug::Print("IRQ %x | ", status);
	}

	PIC::InterruptDone(global_e->irq);

	_asm popad
	_asm iretd
}


void E1000::InitRX()
{
	uint32 ptr;
	struct E1000_RX_DESC *descs;
	//ptr = (uint32)(kmalloc(sizeof(struct E1000_RX_DESC)*NUM_RX_DESC + 16));
	ptr = (uint32)(new uint8[sizeof(E1000_RX_DESC)*NUM_RX_DESC + 16]);

	if (ptr % 16 != 0)
		ptr = (ptr + 16) - (ptr % 16);

	descs = (E1000_RX_DESC*)ptr;
	for (int i = 0; i < NUM_RX_DESC; i++)
	{
		rx_descs[i] = descs++;
		//rx_descs[i]->addr = (uint64)(uint32)(kmalloc(8192 + 16));
		rx_descs[i]->addr = (uint64)(new uint8[8192 + 16]);
		rx_descs[i]->status = 0;
	}

	WriteCommand(REG_RXDESCLO, ptr);
	WriteCommand(REG_RXDESCHI, 0);

	WriteCommand(REG_RXDESCLEN, NUM_RX_DESC * sizeof(E1000_RX_DESC));

	WriteCommand(REG_RXDESCHEAD, 0);
	WriteCommand(REG_RXDESCTAIL, NUM_RX_DESC);
	rx_cur = 0;

	uint32 flags = (2 << 16) | (1 << 25) | (1 << 26) | (1 << 15) | (1 << 5) | (0 << 8) | (0 << 4) | (0 << 3) | (1 << 2);
	WriteCommand(REG_RCTRL, flags);
}

void E1000::InitTX()
{
	uint32 ptr;
	E1000_TX_DESC *descs;
	//ptr = (uint32)(kmalloc(sizeof(struct E1000_TX_DESC)*NUM_TX_DESC + 16));
	ptr = (uint32)(new uint8[sizeof(E1000_RX_DESC)*NUM_RX_DESC + 16]);

	if (ptr % 16 != 0)
		ptr = (ptr + 16) - (ptr % 16);

	descs = (E1000_TX_DESC*)ptr;
	for (int i = 0; i < NUM_TX_DESC; i++)
	{
		tx_descs[i] = descs++;
		tx_descs[i]->addr = 0;
		tx_descs[i]->cmd = 0;
	}

	WriteCommand(REG_TXDESCLO, ptr);
	WriteCommand(REG_TXDESCHI, 0);

	WriteCommand(REG_TXDESCLEN, NUM_TX_DESC * sizeof(E1000_RX_DESC));

	WriteCommand(REG_TXDESCHEAD, 0);
	WriteCommand(REG_TXDESCTAIL, NUM_TX_DESC);
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
	outl(io_base, addr);
	outl(io_base + 4, val);
}

uint32 E1000::ReadCommand(uint16 addr)
{
	outl(io_base, addr);
	return inl(io_base + 4);
}

void E1000::ReadMac()
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