#include "window.h"
#include "client.h"
#include "GUI/messages.h"

namespace gui
{
    static Element* active_element = 0;

    static int mouse_x = 0;
    static int mouse_y = 0;

    Window::Window(char* title, int width, int height, bool capture)
    {
        Client::Init();

        CREATE_WINDOW_RESPONSE response;
        if (Client::SendRequest(CREATE_WINDOW_REQUEST(title, width, height, capture), response))
        {
            this->id = response.id;
            this->width = response.width;
            this->height = response.height;
            this->bounds = Rect(0, 0, width, height);
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
            InputParser::Update(input->code, input->pressed);

            if (input->code == KEY_LBUTTON || input->code == KEY_RBUTTON || input->code == KEY_MBUTTON)
            {
                Element* elem = GetElementAt(mouse_x, mouse_y, this);

                if (input->pressed)
                {                   
                    if (active_element)
                        active_element->isActive = false;

                    if (elem)
                        elem->isActive = true;

                    active_element = elem;

                    if (elem)
                    {
                        elem->Focus();
                        elem->MouseDown();
                    }
                }
                else
                {
                    if (elem)
                        elem->MouseUp();
                }
            }
            else
            {
                KEY_PACKET packet = InputParser::GetPacket();

                if (input->pressed)
                {
                    if (active_element)
                    {
                        active_element->KeyDown(packet);
                        active_element->KeyPress(packet);
                    }
                }
                else
                {
                    if (active_element)
                    {
                        active_element->KeyUp(packet);
                    }
                }
            }
        }
        else if (type == REQUEST_TYPE_MOUSE_INPUT)
        {
            MOUSE_INPUT_MESSAGE* input = (MOUSE_INPUT_MESSAGE*)msg.data;

            mouse_x = input->x;
            mouse_y = input->y;

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

    Element* Window::GetElementAt(int x, int y, GUIBase* parent)
    {
        if (!parent->bounds.Contains(x, y))
            return 0;

        for (int i = 0; i < parent->elements.Count(); i++)
        {
            Element* child = (Element*)parent->elements[i];
            bool isInside = child->bounds.Contains(x, y);

            if (isInside)
            {
                return child;
            }
        }

        return 0;
    }
}