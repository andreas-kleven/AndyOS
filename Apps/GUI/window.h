#pragma once
#include "definitions.h"

class Window
{
public:
    uint32* framebuffer;
    int width;
    int height;

    Window(char* title);
    ~Window();
};