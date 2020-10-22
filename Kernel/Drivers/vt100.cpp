#include <Drawing/font.h>
#include <Drivers/vt100.h>
#include <ctype.h>
#include <debug.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

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

    video = *Video::mode;

    void *virt = VMem::KernelAlloc(BYTES_TO_BLOCKS(video.memsize));
    void *phys = (void *)VMem::GetAddress((size_t)virt);
    video.SetFramebuffer(virt, phys);

    width = video.width / 8;
    height = video.height / 16;
    text_buffer = new CircularDataBuffer(width * height);
    active = false;

    ResetColors();
    ClearScreen();
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

void Vt100Driver::Blink(bool blink)
{
    draw_mutex.Aquire();

    if (blink != state.blink) {
        state.blink = blink;
        InvertColors(state.x, state.y);
    }

    draw_mutex.Release();
}

void Vt100Driver::ClearScreen()
{
    draw_mutex.Aquire();

    if (video.framebuffer) {
        memset32(video.framebuffer, BACKGROUND_COLOR, video.width * video.height);

        if (active)
            Video::mode->Draw(video.framebuffer);
    }

    state.blink = false;
    draw_mutex.Release();
}

void Vt100Driver::ClearLine(int line)
{
    draw_mutex.Aquire();

    int stride = video.width * video.depth / 8;
    int offset = stride * 16 * line;

    if (video.framebuffer) {
        memset32((char *)video.framebuffer + offset, BACKGROUND_COLOR, stride * 16 / 4);

        if (active)
            memset32((char *)Video::mode->framebuffer + offset, BACKGROUND_COLOR, stride * 16 / 4);
    }

    state.blink = false;

    draw_mutex.Release();
}

void Vt100Driver::RedrawScreen()
{
    if (!active)
        return;

    draw_mutex.Aquire();
    Video::mode->Draw(video.framebuffer);
    draw_mutex.Release();
}

int Vt100Driver::AddText(const char *text, size_t size)
{
    return text_buffer->Write(text, size);
}

bool Vt100Driver::ParseSequence(char cmd)
{
    char *ptr;
    char *arg = strtok_r(state.escape_buffer, ";", &ptr);

    int count = 0;
    int args[16];

    while (arg) {
        args[count] = atoi(arg);
        count += 1;

        if (count >= (int)(sizeof(args) / sizeof(args[0])))
            return false;

        arg = strtok_r(0, ";", &ptr);
    }

    return HandleSequence(cmd, count, args);
}

bool Vt100Driver::HandleSequence(char cmd, int num_args, int *args)
{
    if (num_args == 0) {
        switch (cmd) {
        case 'A':
            Move(state.x, state.y - 1);
            return true;
        case 'B':
            Move(state.x, state.y + 1);
            return true;
        case 'C':
            Move(state.x + 1, state.y);
            return true;
        case 'D':
            Move(state.x - 1, state.y);
            return true;
        case 'E':
            Move(0, state.y + 1);
            return true;
        case 'H':
            Move(0, 0);
            return true;
        case 'J':
            ClearScreen();
            return true;
        case 'K':
            ClearLine(state.y);
            return true;
        }
    } else if (num_args == 2) {
        int arg0 = args[0] - 1;
        int arg1 = args[1] - 1;

        switch (cmd) {
        case 'f':
        case 'H':
            Move(arg1, arg0);
            return true;
        }
    } else if (num_args == 1) {
        int arg = args[0];

        switch (cmd) {
        case 'A':
            Move(state.x, state.y - arg);
            return true;
        case 'B':
            Move(state.x, state.y + arg);
            return true;
        case 'C':
            Move(state.x + arg, state.y);
            return true;
        case 'D':
            Move(state.x - arg, state.y);
            return true;
        case 'E':
            Move(0, state.y + arg);
            return true;
        case 'F':
            Move(0, state.y - arg);
            return true;
        case 'G':
            Move(arg - 1, state.y);
            return true;

        case 'm':
            switch (arg) {
            case 7:
                state.invert = true;
                uint32 tmp = state.fg;
                state.fg = state.bg;
                state.bg = tmp;
                return true;
            }
            break;
        }
    }

    switch (cmd) {
    case 'm':
        switch (num_args) {
        case 0: {
            ResetColors();
            return true;
        }
        case 1:
        case 2: {
            int arg = args[num_args - 1];
            int offset = 0;

            if (arg == 0) {
                ResetColors();
                return true;
            } else {
                if (arg >= 30 && arg <= 37) {
                    state.fg = GetColor(arg - 30 + offset);
                    return true;
                } else if (arg >= 40 && arg <= 47) {
                    state.bg = GetColor(arg - 40 + offset);
                    return true;
                }
            }
        }
        case 3:
            if (args[1] == 5) {
                uint32 color = GetColor(args[2]);

                if (args[0] == 38) {
                    state.fg = color;
                    return true;
                } else if (args[0] == 48) {
                    state.bg = color;
                    return true;
                }
            }
            break;
        case 5:
            if (args[1] == 2) {
                uint32 color = GetColor(args[2], args[3], args[4]);

                if (args[0] == 38) {
                    state.fg = color;
                    return true;
                } else if (args[0] == 48) {
                    state.bg = color;
                    return true;
                }
            }
            break;
        }

        break;
    }

    return false;
}

void Vt100Driver::DrawText()
{
    draw_mutex.Aquire();

    bool restore_blink = state.blink;

    if (state.blink)
        Blink(false);

    while (!text_buffer->IsEmpty()) {
        char c = 0;
        text_buffer->Read(1, &c);

        if (!c)
            continue;

        if (state.escaped) {
            if (isdigit(c) || c == ';') {
                state.escape_buffer[state.escape_index++] = c;
                state.escape_buffer[state.escape_index] = 0;
            } else if (isalpha(c)) {
                draw_mutex.Aquire();
                state.escaped = false;

                if (!ParseSequence(c)) {
                    Putc('[');

                    for (int i = 0; i < state.escape_index; i++)
                        Putc(state.escape_buffer[i]);

                    Putc(c);

                    kprintf("[%s%c\n", state.escape_buffer, c);
                }

                draw_mutex.Release();

            } else if (state.escape_index > 0 || c != '[') {
                state.escaped = false;
            }

            if (state.escape_index >= ESCAPE_BUFFER_SIZE - 1) {
                state.escaped = false;
            }
        } else {
            if (c == 0x03 || c == 0x1A || c == 0x1C) {
                c += 'A' - 1;
                Putc('^');
                Putc(c);
            } else if (c == 0x1B) // ESC
            {
                state.escaped = true;
                state.escape_buffer[0] = 0;
                state.escape_index = 0;
            } else {
                Putc(c);
            }
        }
    }

    if (restore_blink)
        Blink(true);

    draw_mutex.Release();
}

void Vt100Driver::DrawChar(int x, int y, int c)
{
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 8; j++) {
            uint32 color = state.bg;

            if ((c == -1) || (DEFAULT_FONT[i + 16 * c] >> j) & 1)
                color = state.fg;

            video.SetPixel(x * 8 + (7 - j), y * 16 + i, color);

            if (active)
                Video::mode->SetPixel(x * 8 + (7 - j), y * 16 + i, color);
        }
    }
}

void Vt100Driver::InvertColors(int x, int y)
{
    uint32 fg;
    uint32 bg;

    if (GetScreenColors(x, y, fg, bg) == 1) {
        if (state.blink)
            state.blink_color = bg;

        fg = (bg == state.blink_color) ? FOREGROUND_COLOR : state.blink_color;
    }

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 8; j++) {
            uint32 color = video.GetPixel(x * 8 + (7 - j), y * 16 + i);
            uint32 newcolor = color == fg ? bg : fg;
            video.SetPixel(x * 8 + (7 - j), y * 16 + i, newcolor);

            if (active)
                Video::mode->SetPixel(x * 8 + (7 - j), y * 16 + i, newcolor);
        }
    }
}

int Vt100Driver::GetScreenColors(int x, int y, uint32 &fg, uint32 &bg)
{
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 8; j++) {
            uint32 color = video.GetPixel(x * 8 + (7 - j), y * 16 + i);

            if (i == 0 && j == 0) {
                bg = color;
            } else if (color != bg) {
                fg = color;
                return 2;
            }
        }
    }

    fg = bg;
    return 1;
}

void Vt100Driver::Putc(char c)
{
    if (!video.framebuffer)
        return;

    switch (c) {
    case '\n':
        Move(0, state.y + 1);
        break;

    case '\r':
        Move(0, state.y);
        break;

    case '\t':
        Putc(' ');

        while (state.x % 8)
            Putc(' ');
        break;

    case '\b':
        if (state.x > 0)
            Move(state.x - 1, state.y);

        DrawChar(state.x, state.y, ' ');
        break;

    default:
        DrawChar(state.x, state.y, c);

        if (state.x == width - 1)
            Move(0, state.y + 1);
        else
            Move(state.x + 1, state.y);
        break;
    }
}

void Vt100Driver::Move(int x, int y)
{
    draw_mutex.Aquire();

    bool restore_blink = state.blink;

    if (state.blink)
        Blink(false);

    state.x = clamp(x, 0, width - 1);
    state.y = max(y, 0);

    if (state.y > height - 1) {
        state.x = 0;
        state.y -= 1;

        int stride = video.width * video.depth / 8;
        int offset = stride * 16;
        int count = stride * 16 * (height - 1);
        char *start = (char *)video.framebuffer + offset;

        memcpy(video.framebuffer, start, count);
        memset32((char *)video.framebuffer + count, BACKGROUND_COLOR, offset / 4);

        if (active)
            RedrawScreen();
    }

    if (restore_blink)
        Blink(true);

    draw_mutex.Release();
}

uint32 Vt100Driver::GetColor(int number)
{
    return color_map[number % 256];
}

uint32 Vt100Driver::GetColor(int r, int g, int b)
{
    return (r << 16) | (g << 8) | (b);
}

void Vt100Driver::ResetColors()
{
    state.fg = FOREGROUND_COLOR;
    state.bg = BACKGROUND_COLOR;
    state.invert = false;
}
