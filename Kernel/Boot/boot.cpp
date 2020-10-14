#include <Boot/multiboot.h>
#include <Drawing/vbe.h>
#include <globals.h>
#include <kernel.h>
#include <types.h>

extern size_t __START_CTORS, __END_CTORS;

void call_ctors()
{
    size_t *ctors = (size_t *)&__START_CTORS;
    int ctor_count = ((size_t)&__END_CTORS - (size_t)&__START_CTORS) / sizeof(size_t);

    for (int i = 1; i < ctor_count - 1; i++) {
        void (*func)(void) = (void (*)(void))ctors[i];
        func();
    }
}

extern "C" void grub_main(MULTIBOOT_INFO *bootinfo, size_t stack_phys, size_t stack_size)
{
    call_ctors();

    size_t mem_size = bootinfo->mem_upper * 0x400;

    VBE_MODE_INFO *mode_info = (VBE_MODE_INFO *)bootinfo->vbe_mode_info;
    VBEVideoMode video_mode = VBEVideoMode(mode_info);

    Kernel::Setup(mem_size, stack_phys, stack_size, &video_mode);
}
