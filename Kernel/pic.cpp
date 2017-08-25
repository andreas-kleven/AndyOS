#include "pic.h"
#include "hal.h"

STATUS PIC::Init()
{
	uint8 icw = 0;

	_asm cli

	icw = (icw & ~PIC_ICW1_MASK_INIT) | PIC_ICW1_INIT_YES;
	icw = (icw & ~PIC_ICW1_MASK_IC4) | PIC_ICW1_IC4_EXPECT;

	SendCommand(icw, 0);
	SendCommand(icw, 1);

	SendData(PIT_BASE0, 0);
	SendData(PIT_BASE1, 1);

	SendData(0x04, 0);
	SendData(0x02, 1);

	icw = (icw & ~PIC_ICW4_MASK_UPM) | PIC_ICW4_UPM_86MODE;

	SendData(icw, 0);
	SendData(icw, 1);

	return STATUS_SUCCESS;
}

void PIC::InterruptDone(uint16 picNum)
{
	if (picNum > 16)
		return;

	if (picNum >= 8)
		SendCommand(PIC_OCW2_MASK_EOI, 1);

	SendCommand(PIC_OCW2_MASK_EOI, 0);
}

uint8 PIC::ReadData(uint8 picNum)
{
	if (picNum > 1)
		return 0;

	uint8 reg = (picNum == 1) ? PIC2_REG_DATA : PIC1_REG_DATA;
	return inb(reg);
}

void PIC::SendData(uint16 data, uint8 picNum)
{
	if (picNum > 1)
		return;

	uint8 reg = (picNum == 1) ? PIC2_REG_DATA : PIC1_REG_DATA;
	outb(reg, data);
}

void PIC::SendCommand(uint8 cmd, uint8 picNum)
{
	if (picNum > 1)
		return;

	uint8	reg = (picNum == 1) ? PIC2_REG_COMMAND : PIC1_REG_COMMAND;
	outb(reg, cmd);
}
