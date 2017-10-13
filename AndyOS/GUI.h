#pragma once
#include "Window.h"

namespace gui
{
	static class WindowManager
	{
	public:
		static STATUS Init();
		static void Start();

		static Window* CreateWindow(char* title);

	private:
		static void UpdateLoop();

		static void PaintDesktop();
		static void PaintWindows();
		static void PaintTaskbar();
		static void PaintCursor();

		static void HandleMouseInput();

		static Window* GetWindowAtCursor();
		static void SetActiveWindow(int id);
	};
}