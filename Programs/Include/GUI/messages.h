#pragma once
#include <AndyOS.h>
#include <sys/drawing.h>
#include "sys/types.h"
#include "string.h"

namespace gui
{
    namespace messages
    {
        #define GUI_MESSAGE_TYPE 1

        enum REQUEST_TYPE
        {
            REQUEST_TYPE_CONNECT,
            REQUEST_TYPE_CREATE_WINDOW,
            REQUEST_TYPE_PAINT,
            REQUEST_TYPE_KEY_INPUT,
            REQUEST_TYPE_MOUSE_INPUT,
            REQUEST_TYPE_SET_CAPTURE,
            REQUEST_TYPE_ACTION,
            REQUEST_TYPE_RESIZE
        };

        enum WINDOW_ACTION
        {
            WINDOW_ACTION_CLOSE,
            WINDOW_ACTION_MAXIMIZE,
            WINDOW_ACTION_MINIMZE
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
            int width;
            int height;

            CREATE_WINDOW_REQUEST(char* title, int width, int height)
            { 
                this->type = REQUEST_TYPE_CREATE_WINDOW; 
                strcpy(this->title, title);

                this->width = width;
                this->height = height;
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
            KEYCODE code;
            bool pressed;

            KEY_INPUT_MESSAGE(int id, KEYCODE code, bool pressed)
            {
                this->type = REQUEST_TYPE_KEY_INPUT;
                this->id = id;
                this->code = code;
                this->pressed = pressed;
            }
        };

        struct MOUSE_INPUT_MESSAGE
        {
            REQUEST_TYPE type;
            int id;
            int x;
            int y;
            int dx;
            int dy;

            MOUSE_INPUT_MESSAGE(int id, int x, int y, int dx, int dy)
            {
                this->type = REQUEST_TYPE_MOUSE_INPUT;
                this->id = id;
                this->x = x;
                this->y = y;
                this->dx = dx;
                this->dy = dy;
            }
        };

        struct SET_CAPTURE_REQUEST
        {
            REQUEST_TYPE type;
            int id;
            bool capture;
        
            SET_CAPTURE_REQUEST(int id, bool capture)
            {
                this->type = REQUEST_TYPE_SET_CAPTURE;
                this->id = id;
                this->capture = capture;
            }
        };

        struct RESIZE_MESSAGE
        {
            REQUEST_TYPE type;
            int id;
            int height;
            int width;
        
            RESIZE_MESSAGE(int id, int width, int height)
            {
                this->type = REQUEST_TYPE_RESIZE;
                this->id = id;
                this->width = width;
                this->height = height;
            }
        };

        struct WINDOW_ACTION_MESSAGE
        {
            REQUEST_TYPE type;
            int id;
            WINDOW_ACTION action;
        
            WINDOW_ACTION_MESSAGE(int id, WINDOW_ACTION action)
            {
                this->type = REQUEST_TYPE_ACTION;
                this->id = id;
                this->action = action;
            }
        };
    }
}