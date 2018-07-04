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

        bool isActive;

        Element(Rect bounds);
        Element(int x, int y, int width, int height);

        Rect GetGlobalBounds();
        GC CreateGC() const;

        virtual void Paint() { }
    };
}