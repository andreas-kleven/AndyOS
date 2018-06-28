#include "window.h"
#include "client.h"
#include "GUI/messages.h"

namespace gui
{
    Window::Window(char* title)
    {
        Client::Init();

        CREATE_WINDOW_RESPONSE response;
        if (Client::SendRequest(CREATE_WINDOW_REQUEST(title), response))
        {
            this->id = response.id;
            this->width = response.width;
            this->height = response.height;
            this->gc = GC(response.width, response.height, response.framebuffer);
	        Drawing::Clear(Color::White, this->gc);

            Client::AddWindow(this);
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

    void Window::HandleMessage(MESSAGE& msg)
    {
        REQUEST_TYPE type = *(REQUEST_TYPE*)msg.data;

        if (type == REQUEST_TYPE_KEY_INPUT)
        {
            KEY_INPUT_MESSAGE* input = (KEY_INPUT_MESSAGE*)msg.data;

            if (input->up)
            {
                KeyUp(input->key);
            }
            else
            {
                KeyDown(input->key);
            }
        }
        else if (type == REQUEST_TYPE_MOUSE_INPUT)
        {
            MOUSE_INPUT_MESSAGE* input = (MOUSE_INPUT_MESSAGE*)msg.data;
            HoverElement(this, input->x, input->y);
        }
    }

    void Window::HoverElement(GUIBase* elem, int x, int y)
    {
        for (int i = 0; i < elem->elements.Count(); i++)
        {
            Element* child = (Element*)elem->elements[i];
            bool isInside = child->bounds.Contains(x, y);

            if (isInside != child->isHovering)
            {
                HoverElement(child, x, y);
            }
        }

        if (elem != this)
        {
            elem->isHovering = ((Element*)elem)->bounds.Contains(x, y);;
        }
    }
}