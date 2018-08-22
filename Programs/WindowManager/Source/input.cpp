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

        unsigned char buf[128];
        int size = fread(buf, 1, 4, mouse_file);

        if (size % 4 != 0)
            return;

        for (int i = 0; i < size; i += 4) 
        {
            unsigned char* mb = &buf[i];

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