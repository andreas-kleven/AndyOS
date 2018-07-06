#include "input.h"
#include <AndyOS.h>
#include "stdio.h"

namespace Input
{
    FILE* mouse_file;

    int _x = 0;
    int _y = 0;
    int _left = 0;
    int _right = 0;
    int _middle = 0;
    int _scroll_x = 0;
    int _scroll_y = 0;

    void read_mouse()
    {
        if (!mouse_file)
            return;

        unsigned char buf[4];

        int i = 0;

        if (fread(buf, 4, 1, mouse_file) == 4)
        {
            //Buttons
			_left = buf[0] & 1;
			_right = buf[0] & 2;
			_middle = buf[0] & 4;

            //Position
            int sx = buf[1];
			int sy = buf[2];

			//Sign
			if (buf[0] & 0x10)
				sx |= 0xFFFFFF00;

			if (buf[0] & 0x20)
				sy |= 0xFFFFFF00;

			_x += sx;
			_y += sy;

            //Scroll
            if (buf[3] & 1)
				_scroll_y += buf[3];
			else
				_scroll_x += buf[3];
        }
    }

    void Init()
    {
        mouse_file = fopen("/dev/mouse", "r");
    }

    void GetMouseButtons(bool& left, bool& right, bool& middle)
    {
        read_mouse();

        left = _left;
        right = _right;
        middle = _middle;
    }

    void GetMouseMovement(int& x, int& y)
    {
        read_mouse();

        x = _x;
        y = _y;

        _x = 0;
        _y = 0;
    }
}