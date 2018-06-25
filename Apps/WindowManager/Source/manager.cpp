#include <AndyOS.h>
#include <sys/drawing.h>
#include <sys/msg.h>
#include "GUI/messages.h"
#include "manager.h"
#include "window.h"
#include "message.h"
#include "string.h"
#include "stdio.h"

static uint32 cursor_bitmap[8 * 14] =
{
	0xFF000000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF000000, 0xFF000000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF000000, 0xFFFFFFFF, 0xFF000000, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00, 0x00, 0x00, 0x00,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00, 0x00, 0x00,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00, 0x00,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFF000000, 0x00,
	0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000, 0x00, 0x00, 0x00,
	0xFF000000, 0xFF000000, 0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF000000, 0xFFFFFFFF, 0xFF000000, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF000000, 0xFF000000, 0xFF000000, 0x00, 0x00
};

static Color col_taskbar;
static Color col_desktop_bg;

static GC gc;
static GC gc_background;
static GC gc_taskbar;
static GC gc_cursor;

static BMP* bmp_background;

static int window_count;
static Window* first_window;
static Window* last_window;

static Window* focused_window;
static Window* hover_window;

static int cursor_x;
static int cursor_y;

static bool mouse_click_L;
static bool mouse_click_R;
static bool mouse_click_M;
static bool window_drag = 0;

static MOUSE_CLICK_INFO mouse_click_L_info;
static MOUSE_CLICK_INFO mouse_click_R_info;
static MOUSE_CLICK_INFO mouse_click_M_info;
static WINDOW_DRAG_INFO window_drag_info;

static MESSAGE msg_handler(MESSAGE msg)
{
    if (msg.type == GUI_MESSAGE_TYPE)
	{
		if (msg.size < 4)
			return MESSAGE(0);

		REQUEST_TYPE type = *(REQUEST_TYPE*)msg.data;

		if (type == REQUEST_TYPE_CONNECT)
		{
			debug_print("Client connected %i\n", msg.src_proc);

			BOOL_RESPONSE* response = new BOOL_RESPONSE();
			response->success = true;

			return MESSAGE(GUI_MESSAGE_TYPE, response, sizeof(BOOL_RESPONSE));
		}
		else if (type == REQUEST_TYPE_CREATE_WINDOW)
		{
			CREATE_WINDOW_REQUEST* request = (CREATE_WINDOW_REQUEST*)msg.data;
			debug_print("Create window request: %s\n", request->title);

			Window* wnd = WindowManager::CreateWindow(request->title);

			CREATE_WINDOW_RESPONSE* response = new CREATE_WINDOW_RESPONSE(0, wnd->bounds.width, wnd->bounds.height);
			return MESSAGE(GUI_MESSAGE_TYPE, response, sizeof(BOOL_RESPONSE));
		}
	}

	return MESSAGE(0);
}

void WindowManager::Start()
{
	gc = GC(1024, 768);
	gc_background = GC(gc.width, gc.height);
	gc_taskbar = GC(gc.width, GUI_TASKBAR_HEIGHT);
	gc_cursor = GC(8, 14);

	col_taskbar = Color(0.2, 0.3, 0.5);
	col_desktop_bg = Color(0.5, 0.5, 0.5);

	bmp_background = 0;
	window_count = 0;
	first_window = 0;
	last_window = 0;
	focused_window = 0;
	hover_window = 0;

	set_message(msg_handler);
	UpdateLoop();
}

Window* WindowManager::CreateWindow(char* title)
{
	Window* window = new Window();
	window->title = title;
	window->next = 0;
	window->previous = 0;

	if (window_count)
	{
		window->previous = last_window;
		last_window->next = window;
	}
	else
	{
		first_window = window;
	}

	last_window = window;
	window_count++;
	return window;
}

void WindowManager::CloseWindow(Window* wnd)
{
	wnd->Close();

	if (wnd->next)
		wnd->next->previous = wnd->previous;

	if (wnd->previous)
		wnd->previous->next = wnd->next;

	if (wnd == first_window)
		first_window = wnd->next;

	if (wnd == last_window)
		last_window = wnd->previous;

	delete wnd;
}

void WindowManager::UpdateLoop()
{
	while (1)
	{
		Drawing::Clear(Color::Black, gc);

		HandleMouseInput();
		HandleKeyInput();

		PaintBackground();
		PaintWindows();
		PaintTaskbar();
		PaintCursor();
		Drawing::Draw(gc);
	}
}


void WindowManager::PaintBackground()
{
	Drawing::FillRect(0, 0, gc.width, gc.height, col_desktop_bg, gc_background);
	Drawing::BitBlt(gc_background, 0, 0, gc_background.width, gc_background.height, gc, 0, 0);
}

void WindowManager::PaintWindows()
{
	if (window_count == 0)
		return;

	Window* wnd = last_window;
	while (wnd)
	{
		wnd->Paint();

		Rect& bounds = wnd->bounds;
		GC& src = wnd->gc;
		Drawing::BitBlt(src, 0, 0, src.width, src.height, gc, bounds.x, bounds.y);

		wnd = wnd->previous;
	}
}

void WindowManager::PaintTaskbar()
{
	int width = gc_taskbar.width;
	int height = gc_taskbar.height;
	int y = gc.height - gc_taskbar.height;

	Drawing::FillRect(0, 0, gc_taskbar.width, gc_taskbar.height, col_taskbar, gc_taskbar);
	Drawing::BitBlt(gc_taskbar, 0, 0, gc_taskbar.width, gc_taskbar.height, gc, 0, y);
}

void WindowManager::PaintCursor()
{
	for (int y = 0; y < 14; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			uint32 c = cursor_bitmap[x + y * 8];

			if (c != 0)
			{
				Color col = Color(c);
				Drawing::SetPixel(x, y, col, gc_cursor);
			}
		}
	}

	Drawing::BitBlt(gc_cursor, 0, 0, gc_cursor.width, gc_cursor.height, gc, cursor_x, cursor_y, 1);
}

void WindowManager::HandleMouseInput()
{
	bool left;
	bool right;
	bool middle;

	get_mouse_buttons(left, right, middle);
	get_mouse_pos(cursor_x, cursor_y);

	int time = get_ticks();

	Window* wnd = GetWindowAtCursor();

	if (left)
	{
		if (!mouse_click_L)
		{
			//Mouse down

			mouse_click_L = 1;
			mouse_click_L_info.click_time = time;
			mouse_click_L_info.click_x = cursor_x;
			mouse_click_L_info.click_y = cursor_y;

			mouse_click_L_info.window = wnd;

			if (wnd)
			{
				if (focused_window == 0 || wnd->id != focused_window->id)
					SetFocusedWindow(wnd);

			}
		}
		else
		{
			//Drag

			int dx = cursor_x - mouse_click_L_info.click_x;
			int dy = cursor_y - mouse_click_L_info.click_y;

			if (wnd)
			{
				if (!window_drag && (dx != 0 || dy != 0))
				{
					if (Rect(wnd->bounds.x, wnd->bounds.y, wnd->bounds.width, GUI_TITLEBAR_HEIGHT)
						.Contains(mouse_click_L_info.click_x, mouse_click_L_info.click_y))
					{
						window_drag = 1;
						window_drag_info.window = wnd;
						window_drag_info.start_x = wnd->bounds.x;
						window_drag_info.start_y = wnd->bounds.y;
					}
				}
			}

			if (window_drag)
			{
				window_drag_info.window->bounds.x = window_drag_info.start_x + dx;
				window_drag_info.window->bounds.y = window_drag_info.start_y + dy;
			}
		}
	}
	else
	{
		if (mouse_click_L)
		{
			//Mouse up

			MOUSE_CLICK_INFO& info = mouse_click_L_info;

			mouse_click_L = 0;
			info.rel_time = time;
			info.rel_x = cursor_x;
			info.rel_y = cursor_y;

			if (window_drag)
			{
				window_drag = 0;
			}
			else
			{

			}
		}
	}
}

void WindowManager::HandleKeyInput()
{

}


Window* WindowManager::GetWindowAtCursor()
{
	Window* wnd = first_window;
	while (wnd)
	{
		if (wnd->bounds.Contains(cursor_x, cursor_y))
			return wnd;

		wnd = wnd->next;
	}

	return 0;
}

void WindowManager::SetFocusedWindow(Window* new_focused)
{
	focused_window = new_focused;

	if (!focused_window->focused)
	{
		if (focused_window != first_window)
		{
			if (focused_window == last_window && focused_window->previous)
				last_window = focused_window->previous;

			//Disconnect
			if (focused_window->previous)
				focused_window->previous->next = focused_window->next;
			if (focused_window->next)
				focused_window->next->previous = focused_window->previous;

			//Insert first
			focused_window->next = first_window;
			focused_window->previous = 0;

			if (first_window->next)
				first_window->next->previous = first_window;
			first_window->previous = focused_window;
			first_window = focused_window;
		}

		focused_window->SetFocus(1);
	}

	Window* wnd = first_window;
	while (wnd)
	{
		if (wnd != focused_window && wnd->focused)
		{
			wnd->SetFocus(0);
		}

		wnd = wnd->next;
	}
}