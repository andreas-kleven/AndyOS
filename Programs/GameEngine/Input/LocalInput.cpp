#include "LocalInput.h"
#include "Input.h"
#include <AndyOS.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

namespace LocalInput {

int mouse_fd = 0;
int keyboard_fd = 0;
bool extended;
KEYCODE extended_scancodes[256];
bool keystates[MAX_KEYS];
float axes[4];

void ReadMouse()
{
    if (!mouse_fd)
        return;

    char buf[256];
    int size = read(mouse_fd, buf, sizeof(buf));

    if (size == -1 || size % 4 != 0)
        return;

    for (int i = 0; i < size; i += 4) {
        unsigned char *mb = (unsigned char *)&buf[i];

        // Buttons
        keystates[KEY_LBUTTON] = mb[0] & 1;
        keystates[KEY_RBUTTON] = mb[0] & 2;
        keystates[KEY_MBUTTON] = mb[0] & 4;

        // Position
        int sx = mb[1];
        int sy = mb[2];

        // Sign
        if (mb[0] & 0x10)
            sx |= 0xFFFFFF00;

        if (mb[0] & 0x20)
            sy |= 0xFFFFFF00;

        axes[AXIS_X] += sx;
        axes[AXIS_Y] += sy;

        // Scroll
        if (mb[3] & 1)
            axes[AXIS_SCROLLX] += mb[3];
        else
            axes[AXIS_SCROLLY] += mb[3];
    }
}

void HandleScancode(uint8_t scan)
{
    uint8_t code;

    if (scan == 0xE0 || scan == 0xE1) {
        extended = true;
    } else {
        if (extended) {
            extended = false;
            code = extended_scancodes[scan & ~0x80];
        } else {
            code = scancodes[scan & ~0x80];
        }

        bool pressed = !(scan & 0x80);

        if (!pressed)
            scan &= ~0x80;

        keystates[code] = pressed;
    }
}

void ReadKeyboard()
{
    if (!keyboard_fd)
        return;

    char buf[256];
    int size = read(keyboard_fd, buf, sizeof(buf));

    if (size == -1)
        return;

    for (int i = 0; i < size; i++)
        HandleScancode((uint8_t)buf[i]);
}

void Update()
{
    ReadMouse();
    ReadKeyboard();
}

void Init()
{
    memset(keystates, 0, sizeof(keystates));
    memset(axes, 0, sizeof(axes));

    mouse_fd = open("/dev/mouse", O_RDONLY | O_NONBLOCK);
    keyboard_fd = open("/dev/keyboard", O_RDONLY | O_NONBLOCK);

    extended_scancodes[0x48] = KEY_UP;
    extended_scancodes[0x49] = KEY_PAGEUP;
    extended_scancodes[0x4B] = KEY_LEFT;
    extended_scancodes[0x4D] = KEY_RIGHT;
    extended_scancodes[0x50] = KEY_DOWN;
    extended_scancodes[0x51] = KEY_PAGEDOWN;
}

bool GetKey(KEYCODE code)
{
    return keystates[code];
}

float GetAxis(INPUT_AXIS axis)
{
    return axes[axis];
}

} // namespace LocalInput
