#include "window.h"
#include "client.h"
#include "GUI/messages.h"

namespace gui
{
    Window::Window(char* title)
    {
        this->parent = 0;

        Client::Init();

        CREATE_WINDOW_RESPONSE response;
        if (Client::SendRequest(CREATE_WINDOW_REQUEST(title), response))
        {
            this->id = response.id;
            this->width = response.width;
            this->height = response.height;
            this->gc = GC(response.width, response.height, response.framebuffer);
	        Drawing::Clear(Color::White, this->gc);
        }
    }

    Window::~Window()
    {

    }

    void Window::Paint()
    {
        for (int i = 0; i < elements.Count(); i++)
        {
            PaintElement(elements[i]);
        }

        Client::SendRequest(PAINT_REQUEST(id, Rect(0, 0, width, height)));
    }

    void Window::PaintElement(GUIBase* elem)
    {
        for (int i = 0; i < elem->elements.Count(); i++)
        {
            PaintElement(elem->elements[i]);
        }

        elem->Paint();
    }
}