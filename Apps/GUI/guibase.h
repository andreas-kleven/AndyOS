#pragma once
#include <AndyOS.h>
#include <sys/drawing.h>

namespace gui
{
    class GUIBase
    {
    public:
        GC gc;
        GUIBase* parent;
        List<GUIBase*> elements;

        void AddChild(GUIBase* child);

        virtual void Paint() { }
    };
}