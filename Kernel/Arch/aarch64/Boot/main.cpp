#include "Arch/uart.h"

extern "C" void aarch64_main()
{
    // set up serial console
    uart_init();
    
    // say hello
    // say hello
    uart_puts("Hello World!\n");
    
    // echo everything back
    while(1) {
        uart_send(uart_getc());
    }
}
