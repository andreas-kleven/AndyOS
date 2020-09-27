#pragma once
#include <AndyOS.h>
#include <andyos/drawing.h>
#include <string.h>
#include <sys/types.h>

namespace gui {
enum MSGID
{
    MSGID_NONE,
    REQID_CREATE_WINDOW,
    REQID_PAINT,
    REQID_SET_CAPTURE,
    RESID_CREATE_WINDOW,
    MSGID_RESIZE,
    MSGID_ACTION,
    MSGID_KEY_INPUT,
    MSGID_MOUSE_INPUT,
};

enum WINDOW_ACTION
{
    WINDOW_ACTION_CLOSE,
    WINDOW_ACTION_MAXIMIZE,
    WINDOW_ACTION_MINIMZE,
};

struct MESSAGE
{
    int type;
    int id;
    char *data;
    int size;
    bool response;
    bool copied = false;

    MESSAGE() {}

    MESSAGE(int type) { this->type = type; }

    MESSAGE(int type, int id, void *data, int size)
    {
        char *newbuf = new char[size];
        memcpy(newbuf, data, size);

        this->type = type;
        this->id = id;
        this->data = newbuf;
        this->size = size;
        this->copied = true;
    }

    MESSAGE(void *buf, int len)
    {
        char *cbuf = (char *)buf;
        this->type = *(int *)&cbuf[0];
        this->id = *(int *)&cbuf[4];
        this->data = &cbuf[8];
        this->size = len - 8;
        this->response = type == RESID_CREATE_WINDOW;
    }
};

struct CREATE_WINDOW_REQUEST
{
    int pid;
    int width;
    int height;
    char title[256];

    CREATE_WINDOW_REQUEST(int pid, const char *title, int width, int height)
    {
        this->pid = pid;
        this->width = width;
        this->height = height;
        strcpy(this->title, title);
    }
};

struct PAINT_REQUEST
{
    Rect bounds;

    PAINT_REQUEST(Rect bounds) { this->bounds = bounds; }
};

struct SET_CAPTURE_REQUEST
{
    bool capture;

    SET_CAPTURE_REQUEST(bool capture) { this->capture = capture; }
};

struct CREATE_WINDOW_RESPONSE
{
    int id;
    uint32_t *framebuffer;
    int width;
    int height;

    CREATE_WINDOW_RESPONSE() {}

    CREATE_WINDOW_RESPONSE(int id, uint32_t *framebuffer, int width, int height)
    {
        this->id = id;
        this->framebuffer = framebuffer;
        this->width = width;
        this->height = height;
    }
};

struct WINDOW_ACTION_MESSAGE
{
    WINDOW_ACTION action;

    WINDOW_ACTION_MESSAGE(WINDOW_ACTION action) { this->action = action; }
};

struct RESIZE_MESSAGE
{
    int height;
    int width;

    RESIZE_MESSAGE(int width, int height)
    {
        this->width = width;
        this->height = height;
    }
};

struct KEY_INPUT_MESSAGE
{
    KEYCODE code;
    bool pressed;

    KEY_INPUT_MESSAGE(KEYCODE code, bool pressed)
    {
        this->code = code;
        this->pressed = pressed;
    }
};

struct MOUSE_INPUT_MESSAGE
{
    int x;
    int y;
    int dx;
    int dy;

    MOUSE_INPUT_MESSAGE(int x, int y, int dx, int dy)
    {
        this->x = x;
        this->y = y;
        this->dx = dx;
        this->dy = dy;
    }
};
} // namespace gui
