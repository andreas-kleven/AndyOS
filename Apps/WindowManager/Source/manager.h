#pragma once
#include "window.h"
#include <sys/msg.h>

#define GUI_TASKBAR_HEIGHT		40

struct MOUSE_CLICK_INFO
{
	Window* window;

	int click_time;
	int click_x;
	int click_y;

	int rel_time;
	int rel_x;
	int rel_y;
};

struct WINDOW_DRAG_INFO
{
	Window* window;

	int start_x;
	int start_y;
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