#pragma once
#include "definitions.h"
#include "string.h"

#define GUI_MESSAGE_TYPE 1

enum REQUEST_TYPE
{
    REQUEST_TYPE_CONNECT,
    REQUEST_TYPE_CREATE_WINDOW
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
    uint32* framebuffer;
    int width;
    int height;

    CREATE_WINDOW_RESPONSE()
    { }

    CREATE_WINDOW_RESPONSE(uint32* framebuffer, int width, int height) 
    { 
        this->type = REQUEST_TYPE_CREATE_WINDOW;
        this->framebuffer = framebuffer;
        this->width = width;
        this->height = height;
    }
};