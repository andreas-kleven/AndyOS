#pragma once

namespace gui
{
	enum WINDOW_MESSAGE_TYPE
	{
		WM_NONE,
		WM_CLOSE,
		WM_PAINT,
		WM_CLICK,

		WM_MOUSEENTER,
		WM_MOUSELEAVE,
		WM_MOUSEDOWN,
		WM_MOUSEUP

	};

	struct WND_MSG
	{
		int id;
		WINDOW_MESSAGE_TYPE type;
		int wParam;
		int lParam;

		WND_MSG()
		{ }

		WND_MSG(int id, WINDOW_MESSAGE_TYPE type, int wParam, int lParam)
		{
			this->id = id;
			this->type = type;
			this->wParam = wParam;
			this->lParam = lParam;
		}
	};

	struct WND_MSG_QUEUE
	{

	};
}