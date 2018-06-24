#include "window.h"
#include "client.h"

Window::Window(char* title)
{
    Client::Init();
    Client::CreateWindow(title);
}

Window::~Window()
{

}