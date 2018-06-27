#pragma once
#include <AndyOS.h>
#include <sys/drawing.h>
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

    private:
        void PaintElement(GUIBase* elem);
    };
}