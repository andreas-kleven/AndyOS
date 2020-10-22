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

static AC97Driver *instance;

AC97Driver::AC97Driver(PciDevice *pci_dev)
{
    this->dev = MKDEV(MAJOR_SOUND, MINOR_SOUND);
    this->name = "ac97";

    this->pci_device = pci_dev;
    this->nambar = pci_dev->config.bar0 & ~1;
    this->nabmbar = pci_dev->config.bar1 & ~1;
    this->irq = pci_dev->config.interruptLine;
    this->lvi = 0;
    this->position = AC97_BDL_LEN - 1;
    this->ready_event.Set();

    size_t bdl_size = AC97_BDL_LEN * sizeof(AC97_BUFFER_ENTRY);
    size_t buffers_size = AC97_BDL_LEN * AC97_BDL_SAMPLES * 2;

    this->bdl = (AC97_BUFFER_ENTRY *)VMem::KernelAlloc(BYTES_TO_BLOCKS(bdl_size), true);
    memset(this->bdl, 0, bdl_size);

    this->buffers = new uint16 *[AC97_BDL_LEN];
    uint16 *buffers = (uint16 *)VMem::KernelAlloc(BYTES_TO_BLOCKS(buffers_size), true);

    for (int i = 0; i < AC97_BDL_LEN; i++) {
        this->buffers[i] = &buffers[i * AC97_BDL_SAMPLES];
        this->bdl[i].buffer = (uint16 *)VMem::GetAddress((size_t)this->buffers[i]);
    }

    // Enable interrupts
    IRQ::Install(0x20 + this->irq, AC97_Interrupt);
    outb(this->nabmbar + AC97_PO_CR, (1 << 3) | (1 << 4));

    // Reset device
    outw(this->nambar + AC97_NAM_RESET, 0);              // resets (any value is possible here)
    outb(this->nabmbar + AC97_NABM_GLB_CTRL_STAT, 0x02); // Also here - 0x02 is however obligatory
    Timer::Sleep(100 * 1000);

    SetVolume(10);
    Timer::Sleep(100 * 1000);

    SetSampleRate();
    Timer::Sleep(100 * 1000);

    // Set buffer
    outl(this->nabmbar + AC97_PO_BDBAR, (uint32)VMem::GetAddress((uint32)this->bdl));
    Timer::Sleep(100 * 1000);

    size_t size = 2;
    uint16 buffer[size];

    WriteBuffer(buffer, size);
    WriteBuffer(buffer, size);

    outb(this->nabmbar + AC97_PO_LVI, AC97_BDL_LEN - 1);
    outb(this->nabmbar + AC97_PO_CR, 25);

    this->status = DRIVER_STATUS_RUNNING;
}

int AC97Driver::Write(FILE *file, const void *buf, size_t size)
{
    // kprintf("Play %d %d, %d\n", lvi, position, (position - lvi + 32) % AC97_BDL_LEN);

    if (size % 2 != 0)
        return -EINVAL;

    size_t count = size / 2;
    size_t max = AC97_BDL_SAMPLES - 2;
    uint16 *ptr = (uint16 *)buf;

    while (count) {
        size_t n = min(count, max);

        int err;
        if ((err = WriteBuffer(ptr, n)))
            return err;

        ptr += n;
        count -= n;
    }

    return size;
}

int AC97Driver::WriteBuffer(void *samples, size_t count)
{
    // TODO: locking

    if (count > AC97_BDL_SAMPLES)
        return -EOVERFLOW;

    if (status == DRIVER_STATUS_RUNNING) {
        if (!ready_event.WaitIntr())
            return -EINTR;
    }

    position = (position + 1) % AC97_BDL_LEN;
    bdl[position].ioc = 1;
    bdl[position].bup = 1;
    bdl[position].length = count;

    uint16 *buffer = buffers[position];
    memcpy(buffer, samples, count * 2);

    if ((position - lvi + 32) % AC97_BDL_LEN > 1)
        ready_event.Clear();

    return 0;
}

int AC97Driver::Ioctl(FILE *file, int request, unsigned int arg)
{
    switch (request) {
    case AC97_IOCTL_SET_VOLUME:
        SetVolume(63 - arg * 64 / 256);
        break;

    default:
        return -EINVAL;
    }

    return 0;
}

void AC97Driver::SetVolume(int volume)
{
    kprintf("Volume: %d\n", volume);

    if (volume < 0)
        volume = 0;

    if (volume > 63)
        volume = 63;

    outw(nambar + AC97_NAM_MASTER_VOLUME, (volume << 8) | volume); // Master (left and right)
    outw(nambar + AC97_NAM_MONO_VOLUME, volume);                   // Mono (vmtl. Unnecessary)
    outw(nambar + AC97_NAM_PC_BEEP, volume);                       // PC loudspeaker
    outw(nambar + AC97_NAM_PCM_VOLUME, (volume << 8) | volume);    // PCM (left and right)
}

void AC97Driver::SetSampleRate()
{
    if (!(inw(this->nambar + AC97_NAM_EXT_AUDIO_ID) & 1)) {
        /*Sample Rate fixed at 48kHz */
        kprintf("Sample rate 48kHz\n");
    } else {
        outw(this->nambar + AC97_NAM_EXT_AUDIO_STC,
             inw(this->nambar + AC97_NAM_EXT_AUDIO_STC) | 1); // Variable Rate Enable audio

        Timer::Sleep(10 * 1000);
        if (!(inw(this->nambar + AC97_NAM_EXT_AUDIO_ID) & 1)) {
            /*Sample Rate fixed at 48kHz */
            kprintf("Sample rate 48kHz\n");
        }

        else {
            outw(this->nambar + AC97_NAM_EXT_AUDIO_STC,
                 inw(this->nambar + AC97_NAM_EXT_AUDIO_STC) | 1);
            outw(this->nambar + AC97_NAM_FRONT_SPLRATE, 44100); // General.
            outw(this->nambar + AC97_NAM_LR_SPLRATE, 44100);    // Stereo Samplerate: 44100 Hz
            // Actual Samplerate is now in AC97_NAM_FRONT_SPLRATE or AC97_NAM_LR_SPLRATE
            kprintf("Sample rate 44.1kHz\n");
        }
    }
}

void AC97Driver::AC97_Interrupt()
{
    uint16 sr = inw(instance->nabmbar + AC97_PO_SR);

    if (!sr)
        return;

    if (sr & AC97_X_SR_LVBCI) {
        // kprintf("1: %d %d\n", this->lvi, this->position);

        outb(instance->nabmbar + AC97_PO_CR, 25);
        outw(instance->nabmbar + AC97_PO_SR, AC97_X_SR_LVBCI);
    } else if (sr & AC97_X_SR_BCIS) {
        instance->lvi = (instance->lvi + 1) % AC97_BDL_LEN;
        // kprintf("2: %d %d\n", instance->lvi, instance->position);

        if (instance->lvi == instance->position) {
            // kprintf("Empty %d\n", instance->lvi);
            int next = (instance->lvi + 1) % AC97_BDL_LEN;
            size_t size = 0x1000;
            memset(instance->buffers[next], 0, size * 2);
            instance->bdl[next].ioc = 1;
            instance->bdl[next].bup = 0;
            instance->bdl[next].length = size;
            instance->position = next;
        }

        outw(instance->nabmbar + AC97_PO_SR, AC97_X_SR_BCIS);

    } else if (sr & AC97_X_SR_FIFOE) {
        kprintf("AC97 FIFOE\n");
    } else {
        kprintf("AC97 ERROR\n");
    }

    if ((instance->position - instance->lvi + 32) % AC97_BDL_LEN <= 1)
        instance->ready_event.Set();
}

void AC97Driver::Init()
{
    PciDevice *pci_dev = PCI::GetDevice(4, 1, -1);

    if (!pci_dev) {
        kprintf("No audio device\n");
        return;
    }

    AC97Driver *driver = new AC97Driver(pci_dev);
    instance = driver;
    DriverManager::AddDriver(driver);
}
