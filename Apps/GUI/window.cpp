#include "window.h"
#include "client.h"
#include "GUI/messages.h"

Window::Window(char* title)
{
    Client::Init();

    CREATE_WINDOW_RESPONSE response;
    if (Client::SendRequest(CREATE_WINDOW_REQUEST(title), response))
    {
        this->width = response.width;
        this->height = response.height;
        this->gc = GC(response.width, response.height, response.framebuffer);
    }
}

Window::~Window()
{

}