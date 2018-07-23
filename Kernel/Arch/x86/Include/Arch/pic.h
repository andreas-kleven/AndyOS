#pragma once
#include "definitions.h"

#define PIT_BASE0 0x20
#define PIT_BASE1 0x28

//! PIC 1 register port addresses
#define PIC1_REG_COMMAND			0x20
#define PIC1_REG_DATA				0x21

//! PIC 2 register port addresses
#define PIC2_REG_COMMAND			0xA0
#define PIC2_REG_DATA				0xA1

#define PIC_ICW1_MASK_IC4			0x1				//00000001
#define PIC_ICW1_MASK_INIT			0x10			//00010000

#define PIC_ICW4_MASK_UPM			0x1				//00000001

#define PIC_ICW1_IC4_EXPECT			1			//1
#define PIC_ICW1_INIT_YES			0x10		//10000

#define PIC_ICW4_UPM_86MODE			1				//1

//! The following devices use PIC 1 to generate interrupts
#define		PIC_IRQ_TIMER			0
#define		PIC_IRQ_KEYBOARD		1
#define		PIC_IRQ_SERIAL2			3
#define		PIC_IRQ_SERIAL1			4
#define		PIC_IRQ_PARALLEL2		5
#define		PIC_IRQ_DISKETTE		6
#define		PIC_IRQ_PARALLEL1		7

//! The following devices use PIC 2 to generate interrupts
#define		PIC_IRQ_CMOSTIMER		0
#define		PIC_IRQ_CGARETRACE		1
#define		PIC_IRQ_AUXILIARY		4
#define		PIC_IRQ_FPU				5
#define		PIC_IRQ_HDC				6

//! Command int 2 bit masks. Use when sending commands
#define		PIC_OCW2_MASK_L1		1		//00000001
#define		PIC_OCW2_MASK_L2		2		//00000010
#define		PIC_OCW2_MASK_L3		4		//00000100
#define		PIC_OCW2_MASK_EOI		0x20	//00100000
#define		PIC_OCW2_MASK_SL		0x40	//01000000
#define		PIC_OCW2_MASK_ROTATE	0x80	//10000000

//! Command int 3 bit masks. Use when sending commands
#define		PIC_OCW3_MASK_RIS		1		//00000001
#define		PIC_OCW3_MASK_RIR		2		//00000010
#define		PIC_OCW3_MASK_MODE		4		//00000100
#define		PIC_OCW3_MASK_SMM		0x20	//00100000
#define		PIC_OCW3_MASK_ESMM		0x40	//01000000
#define		PIC_OCW3_MASK_D7		0x80	//10000000

namespace PIC
{
	void InterruptDone(uint16 picNum);
	STATUS Init();
};