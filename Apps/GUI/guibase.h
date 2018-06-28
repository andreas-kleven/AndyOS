#pragma once
#include <AndyOS.h>
#include <sys/drawing.h>
#include "GUI/messages.h"

namespace gui
{
    class GUIBase
    {
    public:
        GUIBase* parent;
        List<GUIBase*> elements;

        GC gc;
        Rect bounds;

        bool isHovering;

        GUIBase();
        void AddChild(GUIBase* child);

        virtual void Paint() { }

    //protected:
        virtual void KeyDown(KEYCODE key) { };
        virtual void KeyUp(KEYCODE key) { };
        virtual void MouseDown() { };
        virtual void MouseUp() { };

        virtual void MouseEnter() { }
        virtual void MouseLeft() { }
    };
}