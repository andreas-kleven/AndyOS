#pragma once

enum WINDOW_MESSAGE_TYPE
{
	WM_NONE,
	WM_DRAW,
	WM_CLOSE,
	WM_MINIMIZE,
	WM_MAXIMIZE,
	WM_RESIZE,

	WM_MOUSEENTER,
	WM_MOUSELEAVE,
	WM_MOUSEDOWN,
	WM_MOUSEUP,

	WM_KEY_DOWN,
	WM_KEY_UP
};

struct WND_MSG
{
	int id;
	WINDOW_MESSAGE_TYPE type;
	int length;
	char* data;

	WND_MSG()
	{ }

	WND_MSG(int id, WINDOW_MESSAGE_TYPE type, int length, char* data)
	{
		this->id = id;
		this->type = type;
		this->length = length;
		this->data = data;
	}
};