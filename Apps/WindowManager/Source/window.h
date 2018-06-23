#pragma once
#include <sys/drawing.h>
#include "message.h"

#define GUI_WINDOW_BORDER_WIDTH		1
#define GUI_TITLEBAR_HEIGHT			28

enum WINDOW_STATE
{
	WINDOW_STATE_NORMAL,
	WINDOW_STATE_MAXIMIZED,
	WINDOW_STATE_MINIMIZED
};

class Window
{
public:
	Color color_background;
	Color color_foreground;
	Color color_title;

	int id;
	WINDOW_STATE state;
	Rect bounds;
	bool focused;

	GC gc;
	GC gc_content;

	char* title;

	bool b_message = 0;
	WND_MSG message;

	Window* next;
	Window* previous;

	Window();

	void Paint();
	void Close();

	void PaintWindow();

	void SetFocus(bool focus);
	void SetState(WINDOW_STATE state);
};