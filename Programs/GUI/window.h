#pragma once
#include <AndyOS.h>
#include <sys/drawing.h>
#include <sys/msg.h>
#include "Elements/element.h"
#include "sys/types.h"
#include "guibase.h"

namespace gui
{
    class Window : public GUIBase
    {
    public:
        Color background;

        int id;
        int width;
        int height;

        Window(const char* title, int width = 400, int height = 300, Color background = Color::White);
        ~Window();

        void Paint();

        void SetCapture(bool capture);

        void HandleMessage(MESSAGE& msg);

    protected:
        virtual void OnClose() { }
        virtual void OnResize() { }

    private:
        bool isClosed = false;

        void PaintElement(GUIBase* elem);
        void HoverElement(GUIBase* elem, int x, int y);

        Element* GetElementAt(int x, int y, GUIBase* parent);
    };
}