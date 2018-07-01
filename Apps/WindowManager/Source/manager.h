#pragma once
#include "window.h"
#include <sys/msg.h>

struct MOUSE_CLICK_INFO
{
	Window* window = 0;

	int click_time = 0;
	int click_x = 0;
	int click_y = 0;

	int rel_time = 0;
	int rel_x = 0;
	int rel_y = 0;

	int num_clicks = 0;
};

struct WINDOW_DRAG_INFO
{
	Window* window;
	bool resize;
	Rect bounds;
};

class WindowManager
{
public:
	static void Start();

	static void AddWindow(Window* wnd);
	static void CloseWindow(Window* wnd);

private:
	static MESSAGE MessageHandler(MESSAGE msg);
	static void UpdateLoop();

	static void PaintBackground();
	static void PaintWindows();
	static void PaintTaskbar();
	static void PaintCursor();

	static void HandleMouseInput();
	static void HandleKeyInput();

	static Window* GetWindowAtCursor();
	static void SetFocusedWindow(Window* new_active);

	static Window* GetWindow(int id);
};