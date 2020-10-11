#include "input.h"
#include <AndyOS.h>
#include <fcntl.h>
#include <queue>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

struct KEYINPUT
{
    KEYCODE code;
    bool pressed;
};

namespace Input {
int mouse_fd = 0;
int _x = 0;
int _y = 0;
bool _left = 0;
bool _right = 0;
bool _middle = 0;
int _scroll_x = 0;
int _scroll_y = 0;

int keyboard_fd = 0;
bool _extended;
std::queue<KEYINPUT> keyinputs;
KEYCODE extended_scancodes[256];

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
        _left = mb[0] & 1;
        _right = mb[0] & 2;
        _middle = mb[0] & 4;

        // Position
        int sx = mb[1];
        int sy = mb[2];

        // Sign
        if (mb[0] & 0x10)
            sx |= 0xFFFFFF00;

        if (mb[0] & 0x20)
            sy |= 0xFFFFFF00;

        _x += sx;
        _y += sy;

        // Scroll
        if (mb[3] & 1)
            _scroll_y += (signed char)mb[3];
        else
            _scroll_x += (signed char)mb[3];
    }
}

void HandleScancode(uint8_t scan)
{
    uint8_t code;

    if (scan == 0xE0 || scan == 0xE1) {
        _extended = true;
    } else {
        if (_extended) {
            _extended = false;
            code = extended_scancodes[scan & ~0x80];
        } else {
            code = scancodes[scan & ~0x80];
        }

        bool pressed = !(scan & 0x80);

        if (!pressed)
            scan &= ~0x80;

        KEYINPUT input;
        input.code = (KEYCODE)code;
        input.pressed = pressed;
        keyinputs.push(input);
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

void Init()
{
    mouse_fd = open("/dev/mouse", O_RDONLY | O_NONBLOCK);
    keyboard_fd = open("/dev/keyboard", O_RDONLY | O_NONBLOCK);

    extended_scancodes[0x48] = KEY_UP;
    extended_scancodes[0x49] = KEY_PAGEUP;
    extended_scancodes[0x4B] = KEY_LEFT;
    extended_scancodes[0x4D] = KEY_RIGHT;
    extended_scancodes[0x50] = KEY_DOWN;
    extended_scancodes[0x51] = KEY_PAGEDOWN;
}

void GetMouseButtons(bool &left, bool &right, bool &middle)
{
    ReadMouse();

    left = _left;
    right = _right;
    middle = _middle;
}

void GetMouseMovement(int &x, int &y)
{
    ReadMouse();

    x = _x;
    y = _y;

    _x = 0;
    _y = 0;
}

bool GetKeyboardInput(KEYCODE &code, bool &pressed)
{
    ReadKeyboard();

    if (keyinputs.empty())
        return false;

    KEYINPUT input = keyinputs.front();
    keyinputs.pop();
    code = input.code;
    pressed = input.pressed;

    return true;
}
} // namespace Input
