#include <Arch/uart.h>
#include <Arch/mailbox.h>
#include <Arch/mvideo.h>
#include <kernel.h>
#include <debug.h>

extern "C" void aarch64_main()
{
    uart_init();
    uart_puts("Hello\n");

    MVideoMode video_mode(1024, 768, 32);
    Kernel::Setup(0, 0x40000000, &video_mode);
}