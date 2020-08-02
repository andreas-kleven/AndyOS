#include <Drivers/keyboard.h>
#include <Drivers/vtty.h>
#include <hal.h>
#include <io.h>
#include <irq.h>
#include <string.h>
#include <ctype.h>
#include <circbuf.h>
#include <debug.h>

namespace Keyboard
{
	void Keyboard_ISR()
	{
		if (inb(0x64) & 1)
		{
			uint8 scan = inb(0x60);
			VTTY::QueueInput(scan);
		}
	}

	STATUS Init()
	{
		IRQ::Install(KEYBOARD_IRQ, Keyboard_ISR);
		return STATUS_SUCCESS;
	}
} // namespace Keyboard
