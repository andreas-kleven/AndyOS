#pragma once

namespace gui
{
	enum WINDOW_MESSAGE_TYPE
	{
		WM_NONE,
		WM_CLOSE,
		WM_PAINT,
		WM_COMMAND,

		WM_MOUSEENTER,
		WM_MOUSELEAVE,
		WM_MOUSEDOWN,
		WM_MOUSEUP,

		WM_KEY
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

	struct WND_MSG_NODE
	{
		WND_MSG msg;
		WND_MSG_NODE* next = 0;
		WND_MSG_NODE* previous = 0;
	};

	struct WND_MSG_QUEUE
	{
		WND_MSG_NODE* first = 0;
		WND_MSG_NODE* last = 0;

		~WND_MSG_QUEUE()
		{
			WND_MSG_NODE* node = first;
			while (node)
			{
				delete node;
				node = node->next;
			}
		}

		bool Push(WND_MSG msg)
		{
			WND_MSG_NODE* node = new WND_MSG_NODE;
			node->msg = msg;
			
			if (first)
			{
				last->next = node;
				node->previous = last;
				last = node;
			}
			else
			{
				first = node;
				last = node;
			}

			return 1;
		}

		bool Pop(WND_MSG& msg)
		{
			if (first)
			{
				WND_MSG_NODE* node = first;
				msg = node->msg;

				if (node->next)
					node->next->previous = 0;

				first = node->next;
				delete node;

				return 1;
			}

			return 0;
		}
	};
}