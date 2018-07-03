#pragma once
#include "Net/netinterface.h"
#include "HAL/idt.h"

#define E1000_NUM_RX_DESC 128
#define E1000_NUM_TX_DESC 128

struct E1000_RX_DESC
{
	uint64 addr;
	uint16 length;
	uint16 checksum;
	uint8 status;
	uint8 errors;
	uint16 special;
} __attribute__((packed));

struct E1000_TX_DESC
{
	uint64 addr;
	uint16 length;
	uint8 cso;
	uint8 cmd;
	uint8 status;
	uint8 css;
	uint16 special;
} __attribute__((packed));

class E1000 : public NetInterface
{
public:
	E1000(PciDevice* pci_dev);

	virtual void Send(NetPacket* pkt);
	virtual void Poll();
	virtual MacAddress GetMac();
	virtual IPv4Address GetIP();

private:
	uint32 io_base;
	uint32 mem_base;
	uint32 irq;
	uint8 mac[6];

	bool eeprom_exists;

	uint16 rx_cur;
	uint16 tx_cur;

	uint32 rx_virt_addr[E1000_NUM_RX_DESC];

	E1000_RX_DESC* rx_descs[E1000_NUM_RX_DESC];
	E1000_TX_DESC* tx_descs[E1000_NUM_TX_DESC];

	void Start();
	void Linkup();

	void EnableIRQ();
	static void E1000_Interrupt(REGS* regs);

	void InitRX();
	void InitTX();

	bool DetectEEPROM();
	uint32 ReadEEPROM(uint8 addr);

	void WriteCommand(uint16 p_address, uint32 p_value);
	uint32 ReadCommand(uint16 p_address);

	void ReadMac();
};