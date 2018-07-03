#include "ac97.h"
#include "HAL/hal.h"
#include "Lib/debug.h"
#include "string.h"
#include "math.h"

#define AC97_NAM_RESET			0x00
#define AC97_NAM_MASTER_VOLUME	0x02
#define AC97_NAM_MONO_VOLUME	0x06
#define AC97_NAM_PC_BEEP		0x0A
#define AC97_NAM_PCM_VOLUME		0x18
#define AC97_NAM_EXT_AUDIO_ID	0x28
#define AC97_NAM_EXT_AUDIO_STC	0x2A
#define AC97_NAM_FRONT_SPLRATE	0x2C
#define AC97_NAM_LR_SPLRATE		0x32 
#define AC97_NABM_POBDBAR		0x10 
#define AC97_NABM_POLVI			0x15 
#define AC97_NABM_POCONTROL		0x1B 
#define AC97_NABM_GLB_CTRL_STAT 0x60

#define AC97_PO_BDBAR 0x10  /* PCM out buffer descriptor BAR */
#define AC97_PO_CIV   0x14  /* PCM out current index value */
#define AC97_PO_LVI   0x15  /* PCM out last valid index */
#define AC97_PO_SR    0x16  /* PCM out status register */
#define AC97_PO_PICB  0x18  /* PCM out position in current buffer register */
#define AC97_PO_CR    0x1B  /* PCM out control register */

#define AC97_X_SR_DCH   (1 << 0)  /* DMA controller halted */
#define AC97_X_SR_CELV  (1 << 1)  /* Current equals last valid */
#define AC97_X_SR_LVBCI (1 << 2)  /* Last valid buffer completion interrupt */
#define AC97_X_SR_BCIS  (1 << 3)  /* Buffer completion interrupt status */
#define AC97_X_SR_FIFOE (1 << 3)  /* FIFO error */

#define AC97_BDL_LEN              32                    /* Buffer descriptor list length */
#define AC97_BDL_BUFFER_LEN       0x1000                /* Length of buffer in BDL */
#define AC97_CL_GET_LENGTH(cl)    ((cl) & 0xFFFF)       /* Decode length from cl */
#define AC97_CL_SET_LENGTH(cl, v) ((cl) = (v) & 0xFFFF) /* Encode length to cl */
#define AC97_CL_BUP               ((uint32)1 << 30)             /* Buffer underrun policy in cl */
#define AC97_CL_IOC               ((uint32)1 << 31)             /* Interrupt on completion flag in cl */

namespace AC97
{
	uint8* buf;

	AC97_DEVICE device;

	void AC97_ISR(REGS* regs)
	{
		//{
		//	uint8 pi = inb(device.nabmbar + 0x0006) & 0x1C;
		//	uint8 po = inb(device.nabmbar + 0x0016) & 0x1C;
		//	uint8 mc = inb(device.nabmbar + 0x0026) & 0x1C;
		//
		//	outb(device.nabmbar + 0x0006, pi);
		//	outb(device.nabmbar + 0x0016, po);
		//	//outb(device.nabmbar + 0x0026, mc);
		//}

		if (1)
		{
			uint16 sr = inw(device.nabmbar + AC97_PO_SR);
			debug_print("SR: %x   \n", sr);

			if (sr & AC97_X_SR_LVBCI)
			{
				debug_print("1\n");
				outw(device.nabmbar + AC97_PO_SR, AC97_X_SR_LVBCI);
				//outb(device.nabmbar + AC97_PO_LVI, device.lvi);
				//outb(device.nabmbar + AC97_NABM_POCONTROL, 0x15); // Play, and then generate interrupt!
			}
			else if (sr & AC97_X_SR_BCIS)
			{
				debug_print("2\n");
				device.lvi = (device.lvi + 1) % AC97_BDL_LEN;
				outw(device.nabmbar + AC97_PO_LVI, device.lvi);
				outw(device.nabmbar + AC97_PO_SR, AC97_X_SR_BCIS);
			}
			else if (sr & AC97_X_SR_FIFOE)
			{
				debug_print("3\n");
			}
			else
			{
				debug_print("4\n");
				outw(device.nabmbar + AC97_PO_SR, AC97_X_SR_FIFOE);
			}
		}
	}

	STATUS Init(PciDevice* dev)
	{
		device.pci_device = dev;
		device.nambar = dev->config.bar0 & ~1;
		device.nabmbar = dev->config.bar1 & ~1;
		device.irq = dev->config.interruptLine;
		device.bdl = new AC97_BUFFER_ENTRY[AC97_BDL_LEN];
		memset(device.bdl, 0, AC97_BDL_LEN * sizeof(AC97_BUFFER_ENTRY));

		//Enable interrupts
		IDT::InstallIRQ(0x20 + device.irq, (IRQ_HANDLER)AC97_ISR);
		outb(device.nabmbar + AC97_PO_CR, (1 << 3) | (1 << 4));

		debug_print("0x%ux\n", device.nambar);
		debug_print("0x%ux\n", device.nabmbar);
		debug_print("0x%ux\n", device.irq);

		int volume; //Volume; Attention: 0 is full volume, 63 is as good as mute!

					// Code (in any function): 
		outw(device.nambar + AC97_NAM_RESET, 0); // resets (any value is possible here) 
		outb(device.nabmbar + AC97_NABM_GLB_CTRL_STAT, 0x02); // Also here - 0x02 is however obligatory 

		PIT::Sleep(100);

		volume = 15;
		outw(device.nambar + AC97_NAM_MASTER_VOLUME, (volume << 8) | volume); // General. Volume (left and right) 
		outw(device.nambar + AC97_NAM_MONO_VOLUME, volume); // Volume for the mono output (vmtl. Unnecessary) 
		outw(device.nambar + AC97_NAM_PC_BEEP, volume); // volume for the PC loudspeaker (unnecessary if not used) 
		outw(device.nambar + AC97_NAM_PCM_VOLUME, (volume << 8) | volume); // Volume for PCM (left and right) 

		PIT::Sleep(10);

		if (!(inw(device.nambar + AC97_NAM_EXT_AUDIO_ID) & 1))
		{
			/*Sample Rate fixed at 48kHz */
			debug_print("Sample rate 48kHz\n");
		}
		else
		{
			outw(device.nambar + AC97_NAM_EXT_AUDIO_STC, inw(device.nambar + AC97_NAM_EXT_AUDIO_STC) | 1); // Variable Rate Enable audio 

			PIT::Sleep(10);
			if (!(inw(device.nambar + AC97_NAM_EXT_AUDIO_ID) & 1))
			{
				/*Sample Rate fixed at 48kHz */
				debug_print("Sample rate 48kHz\n");
			}

			else
			{
				outw(device.nambar + AC97_NAM_EXT_AUDIO_STC, inw(device.nambar + AC97_NAM_EXT_AUDIO_STC) | 1);
				outw(device.nambar + AC97_NAM_FRONT_SPLRATE, 44100); // General. 
				outw(device.nambar + AC97_NAM_LR_SPLRATE, 44100); // Stereo Samplerate: 44100 Hz 
				PIT::Sleep(10);
				//Actual Samplerate is now in AC97_NAM_FRONT_SPLRATE or AC97_NAM_LR_SPLRATE 
				debug_print("Sample rate 44.1kHz\n");
			}
		}



		// The following may again be in any function ... 
		//int final = 0; // Last valid buffer 
		//
		//uint8** buffers;
		//
		//for (int i = 0; i < 32; i++)
		//{
		//	buffers[i] = new uint8[6553 * 2];
		//	memset(buffers[i], 0, 6553 * 2);
		//
		//	for (int x = 0; x < 6553; x++)
		//		buffers[i][x] = x;
		//}

		//for (int a = 0; a < 0x20000; a++)
		//	device.bdl[i].buffer[a] = a / (i + 1);

		//for (int i = 0; i < size; i++)
		//{
		//	buf[i] = i;
		//}

		/*for (i = 0; (i < 32) && size; i++)
		{
			device.bdl[i].buffer = buffers[i];
			if (size >= 0x20000)  // Even more than 128 KB, so the buffer is full
			{
				// Maximum length is 0xFFFE and NOT 0xFFFF!
				//Left and right // must have the same number of samples, so this number must be even.
				device.bdl[i].length = 0xFFFE;
				size -= 0x20000; // 128 & nbsp; kB away
			}
			else
			{
				// Half the length in bytes because 16-bit samples need two bytes
				device.bdl[i].length = size >> 1;
				size = 0; // Nix more now
			}

			//device.bdl[i].length |= ((uint32)1 << 31);

			device.bdl[i].ioc = 1;

			if (size)  // Another buffer
				device.bdl[i].bup = 0;
			else  // No more buffers
			{
				device.bdl[i].bup = 1;
				final = i; // Last valid buffer is this here
			}

			final = i;
		}*/

		int size = 0x1000;

		for (int i = 0; i < AC97_BDL_LEN; i++)
		{
			//memset(&device.bdl[i], 0, sizeof(AC97_BUFFER_ENTRY));
			device.bdl[i].buffer = new uint8[size];
			memset(device.bdl[i].buffer, 0, size);

			//for (int x = 0; x < size; x++)
			//	device.bdl[i].buffer[x] = sin(x);

			device.bdl[i].length = size;
			device.bdl[i].ioc = 1;
			device.bdl[i].bup = 0;
		}

		device.lvi = 2;
		device.bdl[device.lvi].bup = 1;

		outl(device.nabmbar + AC97_NABM_POBDBAR, (uint32)device.bdl);
		outb(device.nabmbar + AC97_NABM_POLVI, device.lvi);
		outb(device.nabmbar + AC97_NABM_POCONTROL, inb(device.nabmbar + AC97_PO_CR) | 1); // Play, and then generate interrupt!

		return STATUS_SUCCESS;
	}
}