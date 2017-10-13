#pragma once
#include "Window.h"

namespace gui
{
	static class WindowManager
	{
	public:
		static void Start();

		static Window* CreateWindow(char* title);

	private:
		static void UpdateLoop();
		static void DrawDesktop();
		static void Draw();
	};
}