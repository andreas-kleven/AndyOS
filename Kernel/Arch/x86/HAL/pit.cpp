#include "Arch/pit.h"
#include "HAL/hal.h"

namespace PIT
{
	size_t ticks;

	void SendData(uint16 data, uint8 counter)
	{
		uint8 port = (counter == PIT_OCW_COUNTER_0) ? PIT_REG_COUNTER0 :
			((counter == PIT_OCW_COUNTER_1) ? PIT_REG_COUNTER1 : PIT_REG_COUNTER2);

		outb(port, (uint8)data);
	}

	uint8 ReadData(uint8 counter)
	{
		uint8 port = (counter == PIT_OCW_COUNTER_0) ? PIT_REG_COUNTER0 :
			((counter == PIT_OCW_COUNTER_1) ? PIT_REG_COUNTER1 : PIT_REG_COUNTER2);

		return inb(port);
	}

	STATUS Start()
	{
		uint16 divisor = uint16(1193181 / PIT_FREQUENCY);

		uint8 ocw = 0;
		ocw = (ocw & ~PIT_OCW_MASK_MODE) | PIT_OCW_COUNTER_0;
		ocw = (ocw & ~PIT_OCW_MASK_RL) | PIT_OCW_RL_DATA;
		ocw = (ocw & ~PIT_OCW_MASK_COUNTER) | PIT_OCW_MODE_SQUAREWAVEGEN;
		outb(PIT_REG_COMMAND, ocw);

		SendData(divisor & 0xff, 0);
		SendData((divisor >> 8) & 0xff, 0);

		ticks = 0;

		return STATUS_SUCCESS;
	}

	void PIT_ISR(REGS* regs)
	{
		ticks++;
	}

	uint32 Ticks()
	{
		return ticks;
	}

	void Sleep(uint32 time)
	{
		uint32 end = ticks + time;
		while (ticks < end) pause();
	}

	STATUS Init()
	{
		IDT::InstallIRQ(32, PIT_ISR);
		return Start();
	}
}