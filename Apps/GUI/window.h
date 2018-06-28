#pragma once
#include <AndyOS.h>
#include <sys/drawing.h>
#include <sys/msg.h>
#include "Elements/element.h"
#include "definitions.h"
#include "guibase.h"

namespace gui
{
    class Window : public GUIBase
    {
    public:
        int id;
        int width;
        int height;

        Window(char* title);
        ~Window();

        void Paint();

        void HandleMessage(MESSAGE& msg);

    private:
        void PaintElement(GUIBase* elem);
        void HoverElement(GUIBase* elem, int x, int y);

        Element* GetElementAt(int x, int y, GUIBase* parent);
    };
}