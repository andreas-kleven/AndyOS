#include "window.h"
#include "GUI/messages.h"
#include "client.h"
#include <stdlib.h>
#include <unistd.h>

namespace gui {
static Element *active_element = 0;

static int mouse_x = 0;
static int mouse_y = 0;

Window::Window(const char *title, int width, int height, Color background)
{
    this->background = background;

    client::Init();

    CREATE_WINDOW_RESPONSE response;
    if (client::SendRequest(REQID_CREATE_WINDOW, this->id,
                            CREATE_WINDOW_REQUEST(getpid(), title, width, height), response)) {
        this->id = response.id;
        this->width = response.width;
        this->height = response.height;
        this->bounds = Rect(0, 0, width, height);
        this->gc = GC(response.width, response.height, response.framebuffer);
        this->gc.Clear(background);

        client::AddWindow(this);
    } else {
        debug_print("Create window error\n");
        exit(1);
    }
}

Window::~Window()
{}

void Window::Paint()
{
    for (int i = 0; i < elements.size(); i++) {
        PaintElement(elements[i]);
    }

    client::SendRequest(REQID_PAINT, this->id, PAINT_REQUEST(Rect(0, 0, width, height)));
}

void Window::PaintElement(GUIBase *elem)
{
    for (int i = 0; i < elem->elements.size(); i++) {
        PaintElement(elem->elements[i]);
    }

    elem->Paint();
}

void Window::SetCapture(bool capture)
{
    client::SendRequest(REQID_SET_CAPTURE, this->id, SET_CAPTURE_REQUEST(capture));
}

void Window::HandleMessage(const MESSAGE &message)
{
    switch (message.type) {
    case MSGID_KEY_INPUT: {
        KEY_INPUT_MESSAGE *input = (KEY_INPUT_MESSAGE *)message.data;
        InputManager::HandleKey(input->code, input->pressed);

        if (input->code == KEY_LBUTTON || input->code == KEY_RBUTTON ||
            input->code == KEY_MBUTTON) {
            Element *elem = GetElementAt(mouse_x, mouse_y, this);

            if (input->pressed) {
                if (active_element)
                    active_element->isActive = false;

                if (elem)
                    elem->isActive = true;

                active_element = elem;

                if (elem) {
                    elem->Focus();
                    elem->MouseDown();
                }
            } else {
                if (elem)
                    elem->MouseUp();
            }
        } else {
            KEY_PACKET packet = InputManager::GetPacket();
            GUIBase *receiver = active_element;

            if (!active_element)
                receiver = this;

            if (input->pressed) {
                if (receiver) {
                    receiver->KeyDown(packet);
                    receiver->KeyPress(packet);
                }
            } else {
                if (receiver) {
                    receiver->KeyUp(packet);
                }
            }
        }
    } break;

    case MSGID_MOUSE_INPUT: {
        MOUSE_INPUT_MESSAGE *input = (MOUSE_INPUT_MESSAGE *)message.data;

        mouse_x = input->x;
        mouse_y = input->y;

        InputManager::HandleMouse(input->dx, input->dy);
        HoverElement(this, input->x, input->y);
    } break;

    case MSGID_ACTION: {
        WINDOW_ACTION_MESSAGE *msg = (WINDOW_ACTION_MESSAGE *)message.data;

        if (msg->action == WINDOW_ACTION_CLOSE) {
            OnClose();
            isClosed = true;
        }
    } break;

    case MSGID_RESIZE: {
        RESIZE_MESSAGE *msg = (RESIZE_MESSAGE *)message.data;
        this->width = msg->width;
        this->height = msg->height;
        this->gc.Resize(msg->width, msg->height);
        this->gc.Clear(background);

        OnResize();
    } break;
    }

    Paint();
}

void Window::HoverElement(GUIBase *elem, int x, int y)
{
    for (int i = 0; i < elem->elements.size(); i++) {
        Element *child = (Element *)elem->elements[i];
        bool isInside = child->bounds.Contains(x, y);

        if (isInside != child->isHovering) {
            HoverElement(child, x, y);
        }
    }

    if (elem != this) {
        elem->isHovering = ((Element *)elem)->bounds.Contains(x, y);
        ;
    }
}

Element *Window::GetElementAt(int x, int y, GUIBase *parent)
{
    if (!parent->bounds.Contains(x, y))
        return 0;

    for (int i = 0; i < parent->elements.size(); i++) {
        Element *child = (Element *)parent->elements[i];
        bool isInside = child->bounds.Contains(x, y);

        if (isInside) {
            return child;
        }
    }

    return 0;
}
} // namespace gui
