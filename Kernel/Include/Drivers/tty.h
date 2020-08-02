#pragma once
#include <types.h>
#include <driver.h>

class TtyDriver : public CharDriver
{
private:
    CharDriver *driver;

public:
    TtyDriver(CharDriver *driver, int number);

    int Open(FILE *file);
    int Close(FILE *file);
    int Read(FILE *file, void *buf, size_t size);
    int Write(FILE *file, const void *buf, size_t size);
};
