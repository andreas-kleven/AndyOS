#include <Drivers/ac97.h>
#include <Kernel/timer.h>
#include <Process/scheduler.h>
#include <debug.h>
#include <hal.h>
#include <io.h>
#include <irq.h>
#include <math.h>
#include <memory.h>
#include <string.h>

#define AC97_NAM_RESET          0x00
#define AC97_NAM_MASTER_VOLUME  0x02
#define AC97_NAM_MONO_VOLUME    0x06
#define AC97_NAM_PC_BEEP        0x0A
#define AC97_NAM_PCM_VOLUME     0x18
#define AC97_NAM_EXT_AUDIO_ID   0x28
#define AC97_NAM_EXT_AUDIO_STC  0x2A
#define AC97_NAM_FRONT_SPLRATE  0x2C
#define AC97_NAM_LR_SPLRATE     0x32
#define AC97_NABM_GLB_CTRL_STAT 0x60

#define AC97_PO_BDBAR 0x10 /* PCM out buffer descriptor BAR */
#define AC97_PO_CIV   0x14 /* PCM out current index value */
#define AC97_PO_LVI   0x15 /* PCM out last valid index */
#define AC97_PO_SR    0x16 /* PCM out status register */
#define AC97_PO_PICB  0x18 /* PCM out position in current buffer register */
#define AC97_PO_CR    0x1B /* PCM out control register */

#define AC97_X_SR_DCH   (1 << 0) /* DMA controller halted */
#define AC97_X_SR_CELV  (1 << 1) /* Current equals last valid */
#define AC97_X_SR_LVBCI (1 << 2) /* Last valid buffer completion interrupt */
#define AC97_X_SR_BCIS  (1 << 3) /* Buffer completion interrupt status */
#define AC97_X_SR_FIFOE (1 << 4) /* FIFO error */

namespace AC97 {
AC97_DEVICE device;

void AC97_ISR()
{
    uint16 sr = inw(device.nabmbar + AC97_PO_SR);

    if (!sr)
        return;

    if (sr & AC97_X_SR_LVBCI) {
        kprintf("1: %d %d\n", device.lvi, device.position);

        outb(device.nabmbar + AC97_PO_CR, 25);
        outw(device.nabmbar + AC97_PO_SR, AC97_X_SR_LVBCI);
    } else if (sr & AC97_X_SR_BCIS) {
        device.lvi = (device.lvi + 1) % AC97_BDL_LEN;
        kprintf("2: %d %d\n", device.lvi, device.position);

        if (device.lvi == device.position) {
            int next = (device.lvi + 1) % AC97_BDL_LEN;
            size_t size = 0x1000;
            memset(device.buffers[next], 0, size * 2);
            device.bdl[next].ioc = 1;
            device.bdl[next].bup = 0;
            device.bdl[next].length = size;
            device.position = next;
        }

        outw(device.nabmbar + AC97_PO_SR, AC97_X_SR_BCIS);

    } else if (sr & AC97_X_SR_FIFOE) {
        kprintf("AC97 FIFOE\n");
    } else {
        kprintf("AC97 ERROR\n");
    }
}

STATUS Init(PciDevice *pci_dev)
{
    device.pci_device = pci_dev;
    device.nambar = pci_dev->config.bar0 & ~1;
    device.nabmbar = pci_dev->config.bar1 & ~1;
    device.irq = pci_dev->config.interruptLine;
    device.lvi = 0;
    device.position = AC97_BDL_LEN - 1;

    size_t bdl_size = AC97_BDL_LEN * sizeof(AC97_BUFFER_ENTRY);
    size_t buffers_size = AC97_BDL_LEN * AC97_BDL_SAMPLES * 2;

    device.bdl = (AC97_BUFFER_ENTRY *)VMem::KernelAlloc(BYTES_TO_BLOCKS(bdl_size), true);
    memset(device.bdl, 0, bdl_size);

    device.buffers = new uint16 *[AC97_BDL_LEN];
    uint16 *buffers = (uint16 *)VMem::KernelAlloc(BYTES_TO_BLOCKS(buffers_size), true);

    for (int i = 0; i < AC97_BDL_LEN; i++) {
        device.buffers[i] = &buffers[i * AC97_BDL_SAMPLES];
        // kprintf("B %d %p\n", i, device.buffers[i]);
        device.bdl[i].buffer = (uint16 *)VMem::GetAddress((size_t)device.buffers[i]);
    }

    // Enable interrupts
    IRQ::Install(0x20 + device.irq, AC97_ISR);
    outb(device.nabmbar + AC97_PO_CR, (1 << 3) | (1 << 4));

    kprintf("0x%X\n", device.nambar);
    kprintf("0x%X\n", device.nabmbar);
    kprintf("0x%X\n", device.irq);

    int volume; // Volume; Attention: 0 is full volume, 63 is as good as mute!

    // Code (in any function):
    outw(device.nambar + AC97_NAM_RESET, 0);              // resets (any value is possible here)
    outb(device.nabmbar + AC97_NABM_GLB_CTRL_STAT, 0x02); // Also here - 0x02 is however obligatory

    Timer::Sleep(100 * 1000);

    volume = 10;
    outw(device.nambar + AC97_NAM_MASTER_VOLUME,
         (volume << 8) | volume); // General. Volume (left and right)
    outw(device.nambar + AC97_NAM_MONO_VOLUME,
         volume); // Volume for the mono output (vmtl. Unnecessary)
    outw(device.nambar + AC97_NAM_PC_BEEP,
         volume); // volume for the PC loudspeaker (unnecessary if not used)
    outw(device.nambar + AC97_NAM_PCM_VOLUME,
         (volume << 8) | volume); // Volume for PCM (left and right)

    Timer::Sleep(10 * 1000);

    if (!(inw(device.nambar + AC97_NAM_EXT_AUDIO_ID) & 1)) {
        /*Sample Rate fixed at 48kHz */
        kprintf("Sample rate 48kHz\n");
    } else {
        outw(device.nambar + AC97_NAM_EXT_AUDIO_STC,
             inw(device.nambar + AC97_NAM_EXT_AUDIO_STC) | 1); // Variable Rate Enable audio

        Timer::Sleep(10 * 1000);
        if (!(inw(device.nambar + AC97_NAM_EXT_AUDIO_ID) & 1)) {
            /*Sample Rate fixed at 48kHz */
            kprintf("Sample rate 48kHz\n");
        }

        else {
            outw(device.nambar + AC97_NAM_EXT_AUDIO_STC,
                 inw(device.nambar + AC97_NAM_EXT_AUDIO_STC) | 1);
            outw(device.nambar + AC97_NAM_FRONT_SPLRATE, 44100); // General.
            outw(device.nambar + AC97_NAM_LR_SPLRATE, 44100);    // Stereo Samplerate: 44100 Hz
            Timer::Sleep(10 * 1000);
            // Actual Samplerate is now in AC97_NAM_FRONT_SPLRATE or AC97_NAM_LR_SPLRATE
            kprintf("Sample rate 44.1kHz\n");
        }
    }

    outl(device.nabmbar + AC97_PO_BDBAR, (uint32)VMem::GetAddress((uint32)device.bdl));

    size_t size = 64000;
    uint16 *buffer = (uint16 *)VMem::KernelAlloc(BYTES_TO_BLOCKS(size * 2));

    for (size_t x = 0; x < size; x++)
        buffer[x] = (sin((double)x / 40 * 3.1415) + 1) / 2 * 32767;

    Play(buffer, size);
    Play(buffer, size);

    for (size_t x = 0; x < size; x++)
        buffer[x] = (sin((double)x / 20 * 3.1415) + 1) / 2 * 32767;

    outb(device.nabmbar + AC97_PO_LVI, AC97_BDL_LEN - 1);
    outb(device.nabmbar + AC97_PO_CR, 25);

    for (size_t x = 0; x < size; x++)
        buffer[x] = (sin((double)x / 50 * 3.1415) + 1) / 2 * 32767;

    Scheduler::SleepThread(Timer::Ticks() + 3000 * 1000, Scheduler::CurrentThread());
    Play(buffer, size);
    Play(buffer, size);

    for (size_t x = 0; x < size; x++)
        buffer[x] = (sin((double)x / 20 * 3.1415) + 1) / 2 * 32767;

    Play(buffer, size);

    return STATUS_SUCCESS;
}

void WriteSingleBuffer(void *samples, size_t count)
{
    if (count > AC97_BDL_SAMPLES)
        return;

    device.position = (device.position + 1) % AC97_BDL_LEN;
    device.bdl[device.position].ioc = 1;
    device.bdl[device.position].bup = 1;
    device.bdl[device.position].length = count;

    uint16 *buffer = device.buffers[device.position];
    memcpy(buffer, samples, count * 2);

    kprintf("Buf %i %d\n", device.position, count);
}

void Play(void *samples, size_t count)
{
    size_t max = AC97_BDL_SAMPLES - 2;
    uint16 *ptr = (uint16 *)samples;

    while (count) {
        size_t n = min(count, max);
        WriteSingleBuffer(ptr, n);
        ptr += n;
        count -= n;
    }
}
} // namespace AC97
