#pragma once
#include "Window.h"

#include "Label.h"
#include "Button.h"

#define GUI_TASKBAR_HEIGHT		40

namespace gui
{
	struct MOUSE_CLICK_INFO
	{
		Window* window;
		Control* ctrl;

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

	static class WindowManager
	{
	public:
		static STATUS Init();
		static void Start();

		static Window* CreateWindow(char* title);

	private:
		static void UpdateLoop();

		static void PaintBackground();
		static void PaintWindows();
		static void PaintTaskbar();
		static void PaintCursor();

		static void HandleMouseInput();

		static Window* GetWindowAtCursor();
		static void SetFocusedWindow(Window* new_active);
	};
}