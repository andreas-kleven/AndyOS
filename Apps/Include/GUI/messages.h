#pragma once
#include <AndyOS.h>
#include <sys/drawing.h>
#include "definitions.h"
#include "string.h"

#define GUI_MESSAGE_TYPE 1
namespace gui
{
    enum REQUEST_TYPE
    {
        REQUEST_TYPE_CONNECT,
        REQUEST_TYPE_CREATE_WINDOW,
        REQUEST_TYPE_PAINT,
        REQUEST_TYPE_KEY_INPUT,
        REQUEST_TYPE_MOUSE_INPUT
    };

    struct SIMPLE_REQUEST
    {
        REQUEST_TYPE type;

        SIMPLE_REQUEST(REQUEST_TYPE type) 
        { 
            this->type = type; 
        }
    };

    struct CREATE_WINDOW_REQUEST
    {
        REQUEST_TYPE type;
        char title[256];

        CREATE_WINDOW_REQUEST(char* title) 
        { 
            this->type = REQUEST_TYPE_CREATE_WINDOW; 
            strcpy(this->title, title);
        }
    };

    struct BOOL_RESPONSE
    {
        bool success;
    };

    struct CREATE_WINDOW_RESPONSE
    {
        REQUEST_TYPE type;
        int id;
        uint32* framebuffer;
        int width;
        int height;

        CREATE_WINDOW_RESPONSE()
        { }

        CREATE_WINDOW_RESPONSE(int id, uint32* framebuffer, int width, int height) 
        { 
            this->id = id;
            this->type = REQUEST_TYPE_CREATE_WINDOW;
            this->framebuffer = framebuffer;
            this->width = width;
            this->height = height;
        }
    };

    struct PAINT_REQUEST
    {
        REQUEST_TYPE type;
        int id;
        Rect bounds;

        PAINT_REQUEST(int id, Rect bounds)
        {
            this->type = REQUEST_TYPE_PAINT;
            this->id = id;
            this->bounds = bounds;
        }
    };

    struct WINDOW_MESSAGE
    {
        REQUEST_TYPE type;
        int id;
    };

    struct KEY_INPUT_MESSAGE
    {
        REQUEST_TYPE type;
        int id;
        KEYCODE key;
        bool pressed;

        KEY_INPUT_MESSAGE(int id, KEYCODE key, bool pressed)
        {
            this->type = REQUEST_TYPE_KEY_INPUT;
            this->id = id;
            this->key = key;
            this->pressed = pressed;
        }
    };

    struct MOUSE_INPUT_MESSAGE
    {
        REQUEST_TYPE type;
        int id;
        int x;
        int y;

        MOUSE_INPUT_MESSAGE(int id, int x, int y)
        {
            this->type = REQUEST_TYPE_MOUSE_INPUT;
            this->id = id;
            this->x = x;
            this->y = y;
        }
    };
}