void pause()
{
    asm volatile("pause");
}

void enable()
{
    asm volatile("sti");
}

void disable()
{
    asm volatile("cli");
}