#pragma once
#include <Drivers/tty.h>
#include <types.h>
#include <driver.h>
#include <circbuf.h>

class Vt100Driver : public TtyBaseDriver
{
private:
    int width;
    int height;
    int posx;
    int posy;
    uint32 color;
    uint32 bcolor;
    CircularDataBuffer text_buffer;
    bool active;

public:
    Vt100Driver();

    int Open();
    int Close();
    int Write(const void *buf, size_t size);
    int HandleInput(const char *input, size_t size);
    void Activate();
    void Deactivate();

private:
    void ClearScreen();
    void RedrawScreen();
    int AddText(const char *text, size_t size);
    void DrawText();
    void DrawChar(int x, int y, char c);
    void Putc(char c);
};
