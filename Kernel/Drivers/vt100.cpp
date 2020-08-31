#include <Drawing/font.h>
#include <Drivers/vt100.h>
#include <ctype.h>
#include <debug.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <video.h>

// https://github.com/Mayccoll/Gogh/blob/master/themes/cobalt2.sh
#define COLOR_01 0x000000
#define COLOR_02 0xff0000
#define COLOR_03 0x38de21
#define COLOR_04 0xffe50a
#define COLOR_05 0x1460d2
#define COLOR_06 0xff005d
#define COLOR_07 0x00bbbb
#define COLOR_08 0xbbbbbb
#define COLOR_09 0x555555
#define COLOR_10 0xf40e17
#define COLOR_11 0x3bd01d
#define COLOR_12 0xedc809
#define COLOR_13 0x5555ff
#define COLOR_14 0xff55ff
#define COLOR_15 0x6ae3fa
#define COLOR_16 0xffffff

#define BACKGROUND_COLOR 0x132738
#define FOREGROUND_COLOR 0xffffff
#define CURSOR_COLOR     FOREGROUND_COLOR

static uint32 color_map[256] = {COLOR_01, COLOR_02, COLOR_03, COLOR_04, COLOR_05, COLOR_06,
                                COLOR_07, COLOR_08, COLOR_09, COLOR_10, COLOR_11, COLOR_12,
                                COLOR_13, COLOR_14, COLOR_15, COLOR_16};

static bool initialized;

void InitColors()
{
    if (initialized)
        return;

    initialized = true;

    int values[] = {0, 95, 135, 175, 215, 255};
    int index = 16;

    for (uint32 r = 0; r < 6; r++) {
        for (uint32 g = 0; g < 6; g++) {
            for (uint32 b = 0; b < 6; b++) {
                uint32 color = (values[r] << 16) | (values[g] << 8) | (values[b]);
                color_map[index++] = color;
            }
        }
    }

    for (int i = 0; i < 24; i++) {
        uint32 v = 8 + i * 10;
        color_map[index++] = (v << 16) | (v << 8) | (v);
    }
}

Vt100Driver::Vt100Driver()
{
    InitColors();

    width = Video::mode->width / 8;
    height = Video::mode->height / 16;
    text_buffer = new CircularDataBuffer(width * height);
    active = false;
    escaped = false;

    ResetColors();
}

Vt100Driver::~Vt100Driver()
{
    delete text_buffer;
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
    if (Video::mode->framebuffer) {
        memset32(Video::mode->framebuffer, BACKGROUND_COLOR,
                 Video::mode->width * Video::mode->height);
    }
}

void Vt100Driver::RedrawScreen()
{
    state.x = 0;
    state.y = 0;
    text_buffer->Seek(0, SEEK_SET);
    ClearScreen();
    DrawText();
}

int Vt100Driver::AddText(const char *text, size_t size)
{
    return text_buffer->Write(text, size);
}

void Vt100Driver::ParseSequence(char cmd)
{
    char *ptr;
    char *arg = strtok_r(escape_buffer, ";", &ptr);

    int count = 0;
    int args[16];

    while (arg) {
        args[count] = atoi(arg);
        count += 1;

        if (count >= (int)(sizeof(args) / sizeof(args[0])))
            return;

        arg = strtok_r(0, ";", &ptr);
    }

    HandleSequence(cmd, count, args);
}

void Vt100Driver::HandleSequence(char cmd, int num_args, int *args)
{
    switch (cmd) {
    case 'm':
        switch (num_args) {
        case 0:
            ResetColors();
            break;

        case 1:
        case 2: {
            int arg = args[num_args - 1];
            int offset = 0;

            if (arg == 0) {
                ResetColors();
            } else {
                if (arg >= 30 && arg <= 37) {
                    state.fg = GetColor(arg - 30 + offset);
                } else if (arg >= 40 && arg <= 47) {
                    state.bg = GetColor(arg - 40 + offset);
                }
            }
        }
        case 3:
            if (args[1] == 5) {
                uint32 color = GetColor(args[2]);

                if (args[0] == 38) {
                    state.fg = color;
                } else if (args[0] == 48) {
                    state.bg = color;
                }
            }
            break;
        case 5:
            if (args[1] == 2) {
                uint32 color = GetColor(args[2], args[3], args[4]);

                if (args[0] == 38) {
                    state.fg = color;
                } else if (args[0] == 48) {
                    state.bg = color;
                }
            }
            break;

        default:
            return;
        }

        break;
    }
}

void Vt100Driver::DrawText()
{
    if (!active)
        return;

    draw_mutex.Aquire();

    while (!text_buffer->IsEmpty()) {
        char c = 0;
        text_buffer->Read(1, &c);

        if (!c)
            continue;

        if (escaped) {
            if (isdigit(c) || c == ';') {
                escape_buffer[escape_index++] = c;
                escape_buffer[escape_index] = 0;
            } else if (isalpha(c)) {
                ParseSequence(c);
                escaped = false;
            } else if (escape_index > 0 || c != '[') {
                escaped = false;
            }

            if (escape_index >= ESCAPE_BUFFER_SIZE - 1) {
                escaped = false;
            }
        } else {
            if (c == 0x03 || c == 0x1A || c == 0x1C) {
                c += 'A' - 1;
                Putc('^');
                Putc(c);
            }
            if (c == 0x1B) // ESC
            {
                escaped = true;
                escape_buffer[0] = 0;
                escape_index = 0;
            } else {
                Putc(c);
            }
        }
    }

    draw_mutex.Release();
}

void Vt100Driver::DrawChar(int x, int y, char c)
{
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 8; j++) {
            if ((DEFAULT_FONT[i + 16 * c] >> j) & 1)
                Video::SetPixel(x + (8 - j), y + i, state.fg);
            else
                Video::SetPixel(x + (8 - j), y + i, state.bg);
        }
    }
}

void Vt100Driver::Putc(char c)
{
    if (!Video::mode->framebuffer)
        return;

    switch (c) {
    case '\n':
        state.x = 0;
        state.y += 1;
        break;

    case '\r':
        state.x = 0;
        break;

    case '\t':
        Putc(' ');

        while (state.x % 8)
            Putc(' ');
        break;

    case '\b':
        state.x = clamp(state.x - 1, 0, width);
        DrawChar(state.x * 8, state.y * 16, ' ');
        break;

    default:
        DrawChar(state.x * 8, state.y * 16, c);
        state.x++;
        break;
    }

    if (state.x > width - 1) {
        state.x = 0;
        state.y++;
    }

    if (state.y > height - 1) {
        state.y = height - 1;

        const VideoMode *mode = Video::mode;
        int stride = mode->width * mode->depth / 8;
        int offset = stride * 16;
        int count = stride * 16 * (height - 1);
        char *start = (char *)mode->framebuffer + offset;
        memcpy(mode->framebuffer, start, count);
        memset32((char *)mode->framebuffer + count, BACKGROUND_COLOR, offset / 4);
    }
}

uint32 Vt100Driver::GetColor(int number)
{
    return color_map[number % 256] | 0xFF000000;
}

uint32 Vt100Driver::GetColor(int r, int g, int b)
{
    return (r << 16) | (g << 8) | (b) | 0xFF000000;
}

void Vt100Driver::ResetColors()
{
    state.fg = FOREGROUND_COLOR | 0xFF000000;
    state.bg = BACKGROUND_COLOR | 0xFF000000;
}
