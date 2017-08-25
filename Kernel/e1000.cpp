#include "e1000.h"
#include "i825.h"
#include "hal.h"
#include "memory.h"
#include "debug.h"
#include "pic.h"
#include "idt.h"
#include "pit.h"
#include "string.h"

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


uint16 switch_endian16(uint16 nb) {
	return (nb >> 8) | (nb << 8);
}

uint32 switch_endian32(uint32 nb) {
	return ((nb >> 24) & 0xff) |
		((nb << 8) & 0xff0000) |
		((nb >> 8) & 0xff00) |
		((nb << 24) & 0xff000000);
}

struct  arp {
	uint16 type;
	uint16 proto;
	uint8 hlen;
	uint8 plen;
	uint16 op;
	uint8 arp_sha[ETH_ALEN]; // sender hardware address
	uint8 arp_spa[4];        // sender protocol address
	uint8 arp_tha[ETH_ALEN]; // target hardware address
	uint8 arp_tpa[4];        // target protocol address
	uint8 pad[18];
};

struct icmp_packet 
{
	uint8 icmp_type;
	uint8 icmp_code;
	uint16 checksum;
	uint32 rest;
	uint8 data[1024];
};

struct ip_hdr
{
	uint8 ver_ihl;
	uint8 dscp_ecn;
	uint16 length;
	uint16 id;
	uint16 flags_frag;
	uint8 ttl;
	uint8 protocol;
	uint16 checksum;
	uint32 src;
	uint32 dst;
};

struct ethp
{
	uint8 dest[6];
	uint8 src[6];
	uint16 type;
	uint8 payload[1024];
	uint16 data_length;
};

void* kcalloc(int size)
{
	void* p = Memory::AllocBlocks(size / 0x1000 + 1);
	memset(p, 0, size / 0x1000 + 1);
	return p;
}

void* kmalloc(int size)
{
	void* p = Memory::AllocBlocks(size / 0x1000 + 1);
	return p;
}

struct e1000 *e1000_global;

void e1000_outl(struct e1000 *e, uint16 addr, uint32 val)
{
	outl(e->io_base, addr);
	outl(e->io_base + 4, val);
}
void e1000_outb(struct e1000 *e, uint16 addr, uint32 val)
{
	outl(e->io_base, addr);
	outb(e->io_base + 4, val);
}
uint32 e1000_inl(struct e1000 *e, uint16 addr)
{
	outl(e->io_base, addr);
	return inl(e->io_base + 4);
}
uint32 e1000_eeprom_read(struct e1000 *e, uint8 addr)
{
	uint32 val = 0;
	uint32 test;
	if (e->is_e == 0)
		test = addr << 8;
	else
		test = addr << 2;

	e1000_outl(e, REG_EEPROM, test | 0x1);
	if (e->is_e == 0)
		while (!((val = e1000_inl(e, REG_EEPROM)) & (1 << 4)))
			;//	Debug::Print("is %i val %x\n",e->is_e,val);
	else
		while (!((val = e1000_inl(e, REG_EEPROM)) & (1 << 1)))
			;//	Debug::Print("is %i val %x\n",e->is_e,val);
	val >>= 16;
	return val;
}

void e1000_getmac(struct e1000 *e, char *mac)
{
	uint32 temp;
	temp = e1000_eeprom_read(e, 0);
	mac[0] = temp & 0xff;
	mac[1] = temp >> 8;
	temp = e1000_eeprom_read(e, 1);
	mac[2] = temp & 0xff;
	mac[3] = temp >> 8;
	temp = e1000_eeprom_read(e, 2);
	mac[4] = temp & 0xff;
	mac[5] = temp >> 8;
}
void e1000_linkup(struct e1000 *e)
{
	uint32 val;
	val = e1000_inl(e, REG_CTRL);
	e1000_outl(e, REG_CTRL, val | ECTRL_SLU);
}
void e1000_interrupt_enable(struct e1000 *e)
{
	e1000_outl(e, REG_IMASK, 0x1F6DC);
	e1000_outl(e, REG_IMASK, 0xff & ~4);
	e1000_inl(e, 0xc0);
}

#define CMD_EOP                         (1 << 0)    // End of Packet
#define CMD_IFCS                        (1 << 1)    // Insert FCS
#define CMD_IC                          (1 << 2)    // Insert Checksum
#define CMD_RS                          (1 << 3)    // Report Status
#define CMD_RPS                         (1 << 4)    // Report Packet Sent
#define CMD_VLE                         (1 << 6)    // VLAN Packet Enable
#define CMD_IDE                         (1 << 7)    // Interrupt Delay Enable

size_t e1000_send(e1000 *e, uint8*_buf, size_t length)
{
	e->tx_descs[e->tx_cur]->addr = (uint64)_buf;
	e->tx_descs[e->tx_cur]->length = length;
	e->tx_descs[e->tx_cur]->cmd = ((1 << 3) | 3);
	
	uint8 old_cur = e->tx_cur;
	
	e->tx_cur = (e->tx_cur + 1) % NUM_TX_DESC;
	e1000_outl(e, REG_TXDESCTAIL, e->tx_cur);

	while (!(e->tx_descs[old_cur]->status & 0xff));
	return 0;
}

uint32 target_ip = switch_endian32(0xC0A8007B); //192.168.0.123
uint32 local_ip = switch_endian32(0xC0A8007E); //192.168.0.126
//uint32 target_ip = switch_endian32(0xC0A83801); //192.168.56.1
//uint32 local_ip = switch_endian32(0xC0A8387E); //192.168.56.126

struct e1000* global_e;

uint16 ip_checksum(void* vdata, size_t length) {
	// Cast the data pointer to one that can be indexed.
	char* data = (char*)vdata;

	// Initialise the accumulator.
	uint32 acc = 0xffff;

	// Handle complete 16-bit blocks.
	for (size_t i = 0; i + 1<length; i += 2) {
		uint16 word;
		memcpy(&word, data + i, 2);
		acc += switch_endian16(word);
		if (acc>0xffff) {
			acc -= 0xffff;
		}
	}

	// Handle any partial block at the end of the data.
	if (length & 1) {
		uint16 word = 0;
		memcpy(&word, data + length - 1, 1);
		acc += switch_endian16(word);
		if (acc>0xffff) {
			acc -= 0xffff;
		}
	}

	// Return the checksum in network byte order.
	return switch_endian16(~acc);
}

void send_icmp_response(ethp* r_eth)
{
	ip_hdr* r_ip = (ip_hdr*)r_eth->payload;
	uint8 header_length = (r_ip->ver_ihl & 0xF) * 4;
	uint32 icmp_length = switch_endian16(r_ip->length) - header_length;
	icmp_packet* r_icmp = (icmp_packet*)(r_eth->payload + header_length);
	uint32 icmp_data_length = icmp_length - 8;

	icmp_packet* icmp = new icmp_packet;
	icmp->icmp_code = 0;
	icmp->icmp_type = 0;
	icmp->checksum = 0;
	icmp->rest = r_icmp->rest;
	memcpy(icmp->data, r_icmp->data, icmp_data_length);
	icmp->checksum = ip_checksum(icmp, icmp_length);

	ip_hdr* ip = new ip_hdr;
	ip->ver_ihl = r_ip->ver_ihl;
	ip->dscp_ecn = 0;
	ip->length = r_ip->length;
	ip->id = switch_endian16(0);
	ip->flags_frag = switch_endian16(0);
	ip->ttl = 0x80;
	ip->protocol = 1;
	ip->checksum = 0;
	ip->src = local_ip;
	ip->dst = target_ip;
	ip->checksum = ip_checksum(ip, sizeof(ip_hdr));



	ethp* eth = new ethp;
	eth->type = switch_endian16(0x800);
	memcpy(eth->src, global_e->dev->mac, 6);
	memcpy(eth->dest, r_eth->src, 6);
	memcpy(eth->payload, ip, header_length);
	//memcpy(eth->payload + header_length, (uint8*)r_ip + header_length, switch_endian16(ip->length) - header_length);
	memcpy(eth->payload + header_length, icmp, sizeof(icmp_packet));

	//Debug::Print("*** %i\n", switch_endian16(ip->length) - header_length);
	//Debug::Dump(eth, 14 + switch_endian16(ip->length));

	e1000_send(global_e, (uint8*)eth, 14 + switch_endian16(ip->length));
}

void send_arp_request(ethp* eth)
{
	arp* req = (arp*)eth->payload;

	arp a;
	a.type = switch_endian16(1);
	a.proto = switch_endian16(0x800);
	a.hlen = 6;
	a.plen = 4;
	a.op = switch_endian16(2);

	memcpy(a.arp_sha, global_e->dev->mac, 6);
	memcpy(a.arp_tha, req->arp_sha, 6);
	memcpy(a.arp_spa, &local_ip, 4);
	memcpy(a.arp_tpa, req->arp_spa, 4);
	memset(a.pad, 0, 18);

	ethp* p = new ethp;
	memcpy(p->dest, eth->src, 6);
	memcpy(p->src, global_e->dev->mac, 6);
	p->type = switch_endian16(0x806);

	memcpy(p->payload, &a, sizeof(arp));
	p->data_length = sizeof(arp);

	//Debug::Dump(&p, 14 + sizeof(arp) + 18);
	//Debug::Dump(&a, sizeof(arp));

	e1000_send(global_e, (uint8*)p, p->data_length + 14);
}

void e1000_received(struct e1000 *e)
{
	struct sockbuf *sb;
	uint16 old_cur;
	while ((e->rx_descs[e->rx_cur]->status & 0x1))
	{
		uint8* buf = (uint8*)(uint32)(e->rx_descs[e->rx_cur]->addr);
		uint16 len = e->rx_descs[e->rx_cur]->length;

		//sb = sockbuf_alloc(e->dev, len);
		//memcpy(sb->data, buf, len);
		//network_received(sb);

		//Debug::Print("|REC| ");
		//Debug::Dump((uint32*)e->rx_descs[e->rx_cur]->addr, 31);

		E1000_RX_DESC* desc = e->rx_descs[e->rx_cur];
		ethp* eth = (ethp*)desc->addr;
		//eth->data_length = desc->length - 14; switch
		Debug::Print("%x ", switch_endian16(eth->type));

		
		switch (switch_endian16(eth->type))
		{
		case 0x800:
		{
			if (!memcmp(eth->dest, e->dev->mac, 6))
			{
				Debug::Print("PING\n");
				send_icmp_response(eth);
			}
		}
		break;

		case 0x806:
		{
			arp* a = (arp*)eth->payload;

			if (switch_endian16(a->op) == 1)
			{
				if (*(uint32*)a->arp_tpa, local_ip, 4)
				{
					Debug::Print("ARP REQ\n");
					send_arp_request(eth);
				}
			}
			else if (switch_endian16(a->op) == 2)
			{
				if (*(uint32*)a->arp_spa, target_ip, 4)
				{
					Debug::Print("ARP RESP\n");

					//Debug::Print("ARP RESPONSE\n");
					uint32 c = Debug::color;
					Debug::color = 0xFF0000;
					Debug::Dump(a->arp_spa, 6);
					Debug::color = c;
				}
			}
		}
		break;

		default:
			break;
		}

		//Debug::Dump(eth->dest, 6);

		e->rx_descs[e->rx_cur]->status = 0;
		old_cur = e->rx_cur;
		e->rx_cur = (e->rx_cur + 1) % NUM_RX_DESC;
		e1000_outl(e, REG_RXDESCTAIL, old_cur);
	}
}

void e1000_eeprom_gettype(struct e1000 *e)
{
	uint32 val = 0;
	e1000_outl(e, REG_EEPROM, 0x1);

	for (int i = 0; i < 1000; i++)///while( val & 0x2 || val & 0x10)
	{
		val = e1000_inl(e, REG_EEPROM);
		if (val & 0x10)
			e->is_e = 0;
		else
			e->is_e = 1;
	}
}

void e1000_rxinit(struct e1000 *e)
{
	uint32 ptr;
	struct E1000_RX_DESC *descs;
	ptr = (uint32)(kmalloc(sizeof(struct E1000_RX_DESC)*NUM_RX_DESC + 16));
	e->rx_free = (uint8 *)ptr;
	if (ptr % 16 != 0)
		ptr = (ptr + 16) - (ptr % 16);
	descs = (struct E1000_RX_DESC *)ptr;
	for (int i = 0; i < NUM_RX_DESC; i++)
	{
		e->rx_descs[i] = (struct E1000_RX_DESC *)((uint32)descs + i * 16);
		e->rx_descs[i]->addr = (uint64)(uint32)(kmalloc(8192 + 16));
		e->rx_descs[i]->status = 0;
	}

	//give the card the pointer to the descriptors
	e1000_outl(e, REG_RXDESCLO, ptr);
	e1000_outl(e, REG_RXDESCHI, 0);

	//now setup total length of descriptors
	e1000_outl(e, REG_RXDESCLEN, NUM_RX_DESC * 16);

	//setup numbers
	e1000_outl(e, REG_RXDESCHEAD, 0);
	e1000_outl(e, REG_RXDESCTAIL, NUM_RX_DESC);
	e->rx_cur = 0;

	//enable receiving
	//uint32 flags = (2 << 16) | (1 << 25) | (1 << 26) | (1 << 15) | (1 << 5) | (0 << 8) | (1 << 4) | (1 << 3) | ( 1 << 2);
	uint32 flags = (2 << 16) | (1 << 25) | (1 << 26) | (1 << 15) | (1 << 5) | (0 << 8) | (0 << 4) | (0 << 3) | (1 << 2);
	//	uint32 flags = (2 << 16) | (1 << 25) | (1 << 26) | (1 << 15) | (1 << 5) | (0 << 8) | (1 << 4) | ( 1 << 2);
	//	e1000_outl(e, REG_RCTRL, RCTRL_8192 | RCTRL_MPE);
	e1000_outl(e, REG_RCTRL, flags);//RCTRL_8192 | RCTRL_MPE | RCTRL_UPE |RCTRL_EN);
}
void e1000_txinit(struct e1000 *e)
{
	uint32 ptr;
	struct E1000_TX_DESC *descs;
	ptr = (uint32)(kmalloc(sizeof(struct E1000_TX_DESC)*NUM_TX_DESC + 16));
	e->tx_free = (uint8 *)ptr;
	if (ptr % 16 != 0)
		ptr = (ptr + 16) - (ptr % 16);

	descs = (struct E1000_TX_DESC *)ptr;
	for (int i = 0; i < NUM_TX_DESC; i++)
	{
		e->tx_descs[i] = (struct E1000_TX_DESC *)((uint32)descs + i * 16);
		e->tx_descs[i]->addr = 0;
		e->tx_descs[i]->cmd = 0;
	}

	//give the card the pointer to the descriptors
	e1000_outl(e, REG_TXDESCLO, ptr);
	e1000_outl(e, REG_TXDESCHI, 0);

	//now setup total length of descriptors
	e1000_outl(e, REG_TXDESCLEN, NUM_TX_DESC * 16);

	//setup numbers
	e1000_outl(e, REG_TXDESCHEAD, 0);
	e1000_outl(e, REG_TXDESCTAIL, NUM_TX_DESC);
	e->tx_cur = 0;

	e1000_outl(e, REG_TCTRL, (1 << 1) | (1 << 3));
}

void e1000_start(struct e1000 *e)
{
	//set link up
	e1000_linkup(e);
	//have to clear out the multicast filter, otherwise shit breaks
	for (int i = 0; i < 0x80; i++)
		e1000_outl(e, 0x5200 + i * 4, 0);

	e1000_interrupt_enable(e);

	e1000_rxinit(e);
	e1000_txinit(e);
}

void print_mac(char* c)
{
	Debug::Print("Mac: ");
	Debug::Dump(c, 6);
}

void INTERRUPT isr()
{
	_asm pushad

	{
		struct e1000 *e = global_e;
		uint32 status = e1000_inl(e, 0xc0);

		if (status & 0x04)
		{
			e1000_linkup(e);
		}

		if (status & 0x10)
		{
			Debug::Print("Threshold good\n");
		}


		if (status & 0x80)
		{
			e1000_received(e);
		}

		//Debug::Print("IRQ %x | ", status);
	}

	//	e1000_inl(e,0xc0);

	_asm popad
	_asm iretd
}

struct network_dev *e1000_init(PCI_CONFIG_SPACE* pciConfigHeader)
{
	//struct network_dev *device = kcalloc(sizeof(*device), 1);
	struct network_dev *device = (network_dev*)kcalloc(sizeof(*device));
	struct e1000 *e = (struct e1000 *)kmalloc(sizeof(*e));
	e1000_global = e;
	device->device = e;
	e->dev = device;

	//e->pci = pci_get_device(INTEL_VEND, E1000_DEV);
	//if (e->pci == NULL) {
	//	e->pci = pci_get_device(INTEL_VEND, 0x109a);
	//}
	//if (e->pci == NULL) {
	//	e->pci = pci_get_device(INTEL_VEND, 0x100f);
	//}
	uint32 bar_type = pciConfigHeader->command & 1;
	uint32 io_base = pciConfigHeader->BAR2 & ~1;
	uint32 mem_base = pciConfigHeader->BAR2 & ~3;

	global_e = e;

	e->io_base = io_base;
	Debug::Print("I/O base address %x\n", e->io_base);

	//e->mem_base = (uint8 *)(pci_get_bar(e->pci, PCI_BAR_MEM) & ~3);
	//Debug::Print("mem base %x\n",e->mem_base);

	//Debug::Print("IRQ %i PIN %i\n", e->pci_hdr->int_line, e->pci_hdr->int_pin);

	e1000_eeprom_gettype(e);
	e1000_getmac(e, (char*)device->mac);
	print_mac((char*)&device->mac);

	//	for(int i = 0; i < 6; i++)
	//	e1000_outb(e,0x5400 + i, device->mac[i]);	

	//pci_register_irq(e->pci, &e1000_handler, e);
	IDT::SetISR(pciConfigHeader->interruptLine + 0x20, isr);

	e1000_start(e);

	//device->send = e1000_send;
	//	device->receive = e1000_receive;

	uint32 flags = e1000_inl(e, REG_RCTRL);
	e1000_outl(e, REG_RCTRL, flags | RCTRL_EN);//RCTRL_8192 | RCTRL_MPE | RCTRL_UPE |RCTRL_EN);



	arp a;
	a.type = switch_endian16(1);
	a.proto = switch_endian16(0x800);
	a.hlen = 6;
	a.plen = 4;
	a.op = switch_endian16(1);

	memcpy(a.arp_sha, device->mac, 6);
	memset(a.arp_tha, 0, 6);
	memcpy(a.arp_spa, &local_ip, 4);
	memcpy(a.arp_tpa, &target_ip, 4);
	memset(a.pad, 0, 18);

	ethp p;
	p.dest[0] = 0xFF;
	p.dest[1] = 0xFF;
	p.dest[2] = 0xFF;
	p.dest[3] = 0xFF;
	p.dest[4] = 0xFF;
	p.dest[5] = 0xFF;
	memcpy(p.src, device->mac, 6);
	p.type = switch_endian16(0x806);

	memcpy(p.payload, &a, sizeof(arp));
	p.data_length = sizeof(arp);

	Debug::Print("%x %x\n", e->tx_descs, (uint32)e->tx_descs % 16);

	Debug::Dump(&p, 14 + sizeof(arp) + 18);
	//Debug::Dump(&a, sizeof(arp));

	PIT::Sleep(3000);
	e1000_send(e, (uint8*)&p, p.data_length + 14);

	return device;

}