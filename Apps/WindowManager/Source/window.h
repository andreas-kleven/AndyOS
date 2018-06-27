#pragma once
#include <sys/drawing.h>

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
	bool dirty;

	GC gc;

	char* title;

	Window* next;
	Window* previous;

	Window(char* title, int width, int height, uint32* framebuffer);

	void Paint(GC& main_gc);
	void Close();

	void PaintWindow();

	void SetFocus(bool focus);
	void SetState(WINDOW_STATE state);
};