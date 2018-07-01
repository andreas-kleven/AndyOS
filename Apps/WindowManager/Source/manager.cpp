#include <AndyOS.h>
#include <sys/drawing.h>
#include <sys/msg.h>
#include "GUI.h"
#include "manager.h"
#include "window.h"
#include "string.h"
#include "stdio.h"

using namespace gui::messages;

static uint32 cursor_bitmap[8 * 14] =
{
	0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000,
	0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0xFF000000, 0xFF000000, 0x00000000,
	0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000, 0x00000000,
	0xFF000000, 0xFF000000, 0xFF000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0xFF000000, 0xFFFFFFFF, 0xFF000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0xFF000000, 0xFF000000, 0xFF000000, 0x00000000, 0x00000000
};

const int width = 1024;
const int height = 768;

const float sensitivity = 0.5f;

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
static Window* active_window;
static Window* hover_window;

static float cursor_x;
static float cursor_y;

static int cursor_left = false;
static int cursor_right = false;
static int cursor_middle = false;

static bool cursor_enabled;

static bool mouse_click_L;
static bool mouse_click_R;
static bool mouse_click_M;
static bool window_drag = 0;

static MOUSE_CLICK_INFO mouse_click_L_info;
static MOUSE_CLICK_INFO mouse_click_R_info;
static MOUSE_CLICK_INFO mouse_click_M_info;
static WINDOW_DRAG_INFO window_drag_info;

void WindowManager::Start()
{
	gc = GC(width, height);
	gc_background = GC(gc.width, gc.height);
	gc_taskbar = GC(gc.width, GUI_TASKBAR_HEIGHT);
	gc_cursor = GC(8, 14);

	col_taskbar = Color(0.2, 0.3, 0.5);
	col_desktop_bg = Color(0.9, 0.9, 0.9);

	bmp_background = 0;
	window_count = 0;
	first_window = 0;
	last_window = 0;
	focused_window = 0;
	active_window = 0;
	hover_window = 0;

	set_message(MessageHandler);
	UpdateLoop();
}

void WindowManager::AddWindow(Window* wnd)
{
	wnd->next = 0;
	wnd->previous = 0;

	if (window_count)
	{
		wnd->previous = last_window;
		last_window->next = wnd;
	}
	else
	{
		first_window = wnd;
	}

	last_window = wnd;
	window_count++;
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

MESSAGE WindowManager::MessageHandler(MESSAGE msg)
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

			int w = request->width;
			int h = request->height;

			void* addr1;
			void* addr2;

			alloc_shared(msg.src_proc, addr1, addr2, BYTES_TO_BLOCKS(width * height * 4));

			Window* wnd = new Window(msg.src_proc, request->title, w, h, (uint32*)addr1);
			WindowManager::AddWindow(wnd);

			CREATE_WINDOW_RESPONSE* response = new CREATE_WINDOW_RESPONSE(wnd->id, (uint32*)addr2, wnd->gc.width, wnd->gc.height);
			return MESSAGE(GUI_MESSAGE_TYPE, response, sizeof(CREATE_WINDOW_RESPONSE));
		}
		else if (type == REQUEST_TYPE_PAINT)
		{
			PAINT_REQUEST* request = (PAINT_REQUEST*)msg.data;
			Window* wnd = GetWindow(request->id);

			if (wnd)
			{
				wnd->dirty = true;
			}
		}
		else if (type == REQUEST_TYPE_SET_CAPTURE)
		{
			SET_CAPTURE_REQUEST* request = (SET_CAPTURE_REQUEST*)msg.data;
			Window* wnd = GetWindow(request->id);

			wnd->capture = request->capture;
		}
	}

	return MESSAGE(0);
}

void WindowManager::UpdateLoop()
{
	while (1)
	{
		cursor_enabled = !active_window || !active_window->capture;

		HandleMouseInput();
		HandleKeyInput();

		PaintBackground();
		PaintWindows();
		PaintTaskbar();
		
		if (cursor_enabled)
		{
			PaintCursor();
		}

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
		//if (wnd->dirty)
			wnd->Paint(gc);
			
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

	Drawing::BitBlt(gc_cursor, 0, 0, gc_cursor.width, gc_cursor.height, gc, (int)cursor_x, (int)cursor_y, 1);
}

void WindowManager::HandleMouseInput()
{
	bool left;
	bool right;
	bool middle;
	int dx, dy;

	get_mouse_buttons(left, right, middle);
	get_mouse_pos(dx, dy);

	if (cursor_enabled)
	{
		cursor_x = clamp(cursor_x + sensitivity * dx, 0.0f, (float)width);
		cursor_y = clamp(cursor_y - sensitivity * dy, 0.0f, (float)height);
	}

	Window* wnd = GetWindowAtCursor();
	hover_window = wnd;

	if (wnd)
	{
		if (left != cursor_left)
		{
			KEY_INPUT_MESSAGE msg(wnd->id, KEY_LBUTTON, left);
			post_message(wnd->proc_id, GUI_MESSAGE_TYPE, &msg, sizeof(KEY_INPUT_MESSAGE));
		}

		if (right != cursor_right)
		{
			KEY_INPUT_MESSAGE msg(wnd->id, KEY_RBUTTON, right);
			post_message(wnd->proc_id, GUI_MESSAGE_TYPE, &msg, sizeof(KEY_INPUT_MESSAGE));
		}

		if (middle != cursor_middle)
		{
			KEY_INPUT_MESSAGE msg(wnd->id, KEY_MBUTTON, middle);
			post_message(wnd->proc_id, GUI_MESSAGE_TYPE, &msg, sizeof(KEY_INPUT_MESSAGE));
		}
	}

	cursor_left = left;
	cursor_right = right;
	cursor_middle = middle;

	if (dx != 0 || dy != 0)
	{
		if (hover_window)
		{
			int relx = cursor_x - hover_window->bounds.x;
			int rely = cursor_y - hover_window->bounds.y - GUI_TITLEBAR_HEIGHT;
			int _dx = (active_window == hover_window) ? dx : 0;
			int _dy = (active_window == hover_window) ? dy : 0;

			MOUSE_INPUT_MESSAGE msg(hover_window->id, relx, rely, _dx, _dy);
			post_message(hover_window->proc_id, GUI_MESSAGE_TYPE, &msg, sizeof(MOUSE_INPUT_MESSAGE));
		}
	}

	int time = get_ticks();

	if (left)
	{
		if (!mouse_click_L)
		{
			//Mouse down

			mouse_click_L_info.num_clicks += 1;

			if (time - mouse_click_L_info.click_time > DOUBLE_CLICK_TIME)
			{
				mouse_click_L_info.num_clicks = 0;
			}

			mouse_click_L = 1;
			mouse_click_L_info.click_time = time;
			mouse_click_L_info.click_x = cursor_x;
			mouse_click_L_info.click_y = cursor_y;

			mouse_click_L_info.window = wnd;

			if (wnd)
			{
				if (focused_window == 0 || wnd->id != focused_window->id)
				{
					SetFocusedWindow(wnd);
				}

				if (wnd->content_bounds.Contains(cursor_x, cursor_y))
				{
					active_window = wnd;
				}
				else if (mouse_click_L_info.num_clicks == 1)
				{
					if (wnd->state == WINDOW_STATE_NORMAL)
					{
						wnd->SetState(WINDOW_STATE_MAXIMIZED);
					}
					else if (wnd->state == WINDOW_STATE_MAXIMIZED)
					{
						wnd->SetState(WINDOW_STATE_NORMAL);
					}
				}
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
				Window* dragwnd = window_drag_info.window;

				if (dragwnd->state == WINDOW_STATE_MAXIMIZED)
				{
					//Set window state to normal when dragging maximized window

					float ratio = cursor_x / (float)width;
					int nx = cursor_x - dragwnd->normal_bounds.width * ratio;

					dragwnd->normal_bounds.x = nx;
					dragwnd->normal_bounds.y = 0;
					dragwnd->SetState(WINDOW_STATE_NORMAL);

					window_drag_info.start_x = dragwnd->bounds.x;
 					window_drag_info.start_y = dragwnd->bounds.y;
				}

				int x = window_drag_info.start_x + dx;
				int y = window_drag_info.start_y + dy;
				window_drag_info.window->Move(x, y);
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

				Window* dragwnd = window_drag_info.window;

				if (dragwnd)
				{
					if (dragwnd->bounds.y == 0)
						dragwnd->SetState(WINDOW_STATE_MAXIMIZED);
				}
			}
			else
			{

			}
		}
	}
}

void WindowManager::HandleKeyInput()
{
	KEYCODE code;
	bool pressed;

	while (get_last_key(code, pressed))
	{
		gui::InputManager::HandleKey(code, pressed);

		if (focused_window)
		{
			KEY_INPUT_MESSAGE msg(focused_window->id, code, pressed);
			post_message(focused_window->proc_id, GUI_MESSAGE_TYPE, &msg, sizeof(KEY_INPUT_MESSAGE));
		}

		//alt+tab
		if (gui::InputManager::GetKeyDown(KEY_LALT) && gui::InputManager::GetKeyDown(KEY_TAB))
		{
			active_window = 0;
		}
	}
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

Window* WindowManager::GetWindow(int id)
{
	Window* wnd = first_window;

	while (wnd)
	{
		if (wnd->id == id)
			return wnd;

		wnd = wnd->next;
	}

	return 0;
}