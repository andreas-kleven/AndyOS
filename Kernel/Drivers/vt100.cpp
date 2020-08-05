#include <Drivers/vt100.h>
#include <Drawing/font.h>
#include <video.h>
#include <math.h>
#include <string.h>
#include <debug.h>

Vt100Driver::Vt100Driver()
{
    width = Video::mode->width / 8;
    height = Video::mode->height / 16;
    posx = 0;
    posy = 0;
    color = 0xFFFFFFFF;
    bcolor = 0;
    text_buffer = CircularDataBuffer(width * height);
    active = false;
}

int Vt100Driver::Open()
{
    return -1;
}

int Vt100Driver::Close()
{
    return -1;
}

int Vt100Driver::Write(const void *buf, size_t size)
{
    int written = AddText((const char *)buf, size);

    if (written < 0)
        return written;

    DrawText();
    return written;
}

int Vt100Driver::HandleInput(const char *input, size_t size)
{
    return tty->HandleInput(input, size);
}

void Vt100Driver::Activate()
{
    if (active)
        return;

    active = true;
    RedrawScreen();
}

void Vt100Driver::Deactivate()
{
    if (!active)
        return;

    active = false;
}

void Vt100Driver::ClearScreen()
{
    memset(Video::mode->framebuffer, 0, Video::mode->memsize);
}

void Vt100Driver::RedrawScreen()
{
    posx = 0;
    posy = 0;
    text_buffer.Seek(0, SEEK_SET);
    ClearScreen();
    DrawText();
}

int Vt100Driver::AddText(const char *text, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        char c = text[i];

        if (!c)
            continue;

        if (c == 0x03 || c == 0x1A || c == 0x1C)
        {
            c += 'A' - 1;
            char buf[4];
            sprintf(buf, "^%c\n", c);
            text_buffer.Write(buf, 3);
        }
        else
        {
            text_buffer.Write(&c, 1);
        }
    }

    return size;
}

void Vt100Driver::DrawText()
{
    if (!active)
        return;

    while (!text_buffer.IsEmpty())
    {
        char c;
        text_buffer.Read(1, &c);

        if (c)
            Putc(c);
    }
}

void Vt100Driver::DrawChar(int x, int y, char c)
{
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if ((DEFAULT_FONT[i + 16 * c] >> j) & 1)
                Video::SetPixel(x + (8 - j), y + i, color);
            else
                Video::SetPixel(x + (8 - j), y + i, bcolor);
        }
    }
}

void Vt100Driver::Putc(char c)
{
    switch (c)
    {
    case '\n':
        posx = 0;
        posy += 1;
        break;

    case '\r':
        posx = 0;
        break;

    case '\t':
        Putc(' ');

        while (posx % 8)
            Putc(' ');
        break;

    case '\b':
        posx = clamp(posx - 1, 0, width);
        DrawChar(posx * 8, posy * 16, ' ');
        break;

    default:
        DrawChar(posx * 8, posy * 16, c);
        posx++;
        break;
    }

    if (posx > width - 1)
    {
        posx = 0;
        posy++;
    }

    if (posy > height - 1)
    {
        posy = height - 1;

        const VideoMode *mode = Video::mode;
        int stride = mode->width * mode->depth / 8;
        int offset = stride * 16;
        int count = stride * 16 * (height - 1);
        char *start = (char *)mode->framebuffer + offset;
        memcpy(mode->framebuffer, start, count);
        memset((char *)mode->framebuffer + count, 0, offset);
    }
}
