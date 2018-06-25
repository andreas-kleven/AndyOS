#pragma once
#include <sys/drawing.h>
#include "definitions.h"

class Window
{
public:
    GC gc;
    int width;
    int height;

    Window(char* title);
    ~Window();
};