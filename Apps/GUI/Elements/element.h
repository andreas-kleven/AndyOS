#pragma once
#include <sys/drawing.h>
#include "../guibase.h"

namespace gui
{
    class Element : public GUIBase
    {
    public:
        Color foreground;
        Color background;

        Element(Rect bounds);
        Element(int x, int y, int width, int height);

        Rect GetGlobalBounds();
        GC CreateGC() const;

        virtual void Paint() { }

        virtual void MouseDown() { }
        virtual void MouseUp() { }
        virtual void MouseEnter() { }
        virtual void MouseLeft() { }

        virtual void KeyDown() { }
        virtual void KeyUp() { }
    };
}