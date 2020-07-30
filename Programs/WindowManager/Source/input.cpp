#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <AndyOS.h>
#include "input.h"

namespace Input
{
    int mouse_fd;
    int _x = 0;
    int _y = 0;
    bool _left = 0;
    bool _right = 0;
    bool _middle = 0;
    int _scroll_x = 0;
    int _scroll_y = 0;

    void read_mouse()
    {
        if (!mouse_fd)
            return;

        char buf[256];
        int size = read(mouse_fd, buf, sizeof(buf));

        if (size == -1 || size % 4 != 0)
            return;

        for (int i = 0; i < size; i += 4)
        {
            unsigned char *mb = (unsigned char *)&buf[i];

            //Buttons
            _left = mb[0] & 1;
            _right = mb[0] & 2;
            _middle = mb[0] & 4;

            //Position
            int sx = mb[1];
            int sy = mb[2];

            //Sign
            if (mb[0] & 0x10)
                sx |= 0xFFFFFF00;

            if (mb[0] & 0x20)
                sy |= 0xFFFFFF00;

            _x += sx;
            _y += sy;

            //Scroll
            if (mb[3] & 1)
                _scroll_y += mb[3];
            else
                _scroll_x += mb[3];
        }
    }

    void Init()
    {
        mouse_fd = open("/dev/mouse", O_RDONLY | O_NONBLOCK);
    }

    void GetMouseButtons(bool &left, bool &right, bool &middle)
    {
        read_mouse();

        left = _left;
        right = _right;
        middle = _middle;
    }

    void GetMouseMovement(int &x, int &y)
    {
        read_mouse();

        x = _x;
        y = _y;

        _x = 0;
        _y = 0;
    }
} // namespace Input