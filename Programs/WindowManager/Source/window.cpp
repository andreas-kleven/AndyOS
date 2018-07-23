#include "window.h"
#include "GUI.h"
#include "definitions.h"
#include "string.h"

using namespace gui::messages;

static int new_id = 1;

Window::Window(int proc_id, char* title, int width, int height, uint32* framebuffer)
{
	color_background = Color(0, 0.5, 0.5);
	color_foreground = Color::White;
	color_title = Color::Black;

	this->id = new_id++;
	this->proc_id = proc_id;

	this->title = new char[strlen(title) + 1];
	strcpy(this->title, title);

	state = WINDOW_STATE_NORMAL;
	focused = false;
	dirty = true;

	Move(id * 80, id * 60);
	gc = GC(content_bounds.width, content_bounds.height, framebuffer);
	Resize(width, height);
}

void Window::Paint(GC& main_gc)
{
	const int bw = GUI_WINDOW_BORDER_WIDTH;

	//if (dirty)
		Drawing::BitBlt(gc, 0, 0, gc.width, gc.height, main_gc, bounds.x, bounds.y + GUI_TITLEBAR_HEIGHT);

	Color _titlebar = focused ? color_background : Color::White;
	Color _border = focused ? color_background : Color::Black;

	Drawing::FillRect(bounds.x, bounds.y, bounds.width, GUI_TITLEBAR_HEIGHT, _titlebar, main_gc); //Title bar
	Drawing::DrawText(bounds.x + 4, bounds.y + 4, title, color_title, main_gc); //Title

	Drawing::FillRect(bclose_bounds, Color::Red, main_gc);
	Drawing::FillRect(bmin_bounds, Color::Yellow, main_gc);
	Drawing::FillRect(bmax_bounds, Color::Green, main_gc);

	Drawing::DrawRect(bounds.x - bw, bounds.y - bw, bounds.width + bw * 2, bounds.height + bw * 2, bw, _border, main_gc); //Window border

	dirty = false;
}

void Window::Move(int x, int y)
{
	y = max(y, 0);

	bounds.x = x;
	bounds.y = y;
	content_bounds.x = x;
	content_bounds.y = y + GUI_TITLEBAR_HEIGHT;
	titlebar_bounds.x = x;
	titlebar_bounds.y = y;

	UpdateTitleButtons();
}

void Window::Resize(int w, int h)
{
	w = max(w, 100);
	h = max(h, GUI_TITLEBAR_HEIGHT + 30);

	bounds.width = w;
	bounds.height = h;	
	content_bounds.width = w;
	content_bounds.height = h - GUI_TITLEBAR_HEIGHT;

	titlebar_bounds.width = w;
	titlebar_bounds.height = GUI_TITLEBAR_HEIGHT;

	gc.Resize(content_bounds.width, content_bounds.height);

	UpdateTitleButtons();

	RESIZE_MESSAGE msg = RESIZE_MESSAGE(id, gc.width, gc.height);
	post_message(this->proc_id, GUI_MESSAGE_TYPE, &msg, sizeof(msg));
}

void Window::Close()
{
	WINDOW_ACTION_MESSAGE msg = WINDOW_ACTION_MESSAGE(id, WINDOW_ACTION_CLOSE);
	post_message(this->proc_id, GUI_MESSAGE_TYPE, &msg, sizeof(msg));
}

void Window::Minimize()
{
	SetState(WINDOW_STATE_MINIMIZED);
}

void Window::Restore()
{
	SetState(saved_state);
}

void Window::ToggleMaximized()
{
	WINDOW_STATE ws = state;

	if (ws == WINDOW_STATE_MINIMIZED)
		ws = saved_state;

	if (ws == WINDOW_STATE_NORMAL)
	{
		SetState(WINDOW_STATE_MAXIMIZED);
	}
	else if (ws == WINDOW_STATE_MAXIMIZED)
	{
		SetState(WINDOW_STATE_NORMAL);
	}
}

void Window::SetFocus(bool focus)
{
	this->focused = focus;
}

void Window::SetState(WINDOW_STATE ws)
{
	if (ws == state)
		return;

	if (state == WINDOW_STATE_NORMAL)
		normal_bounds = bounds;

	switch (ws)
	{
	case WINDOW_STATE_NORMAL:
		Move(normal_bounds.x, normal_bounds.y);
		Resize(normal_bounds.width, normal_bounds.height);
		break;

	case WINDOW_STATE_MAXIMIZED:
		Move(0, 0);
		Resize(1024, 768 - GUI_TASKBAR_HEIGHT);
		break;

	case WINDOW_STATE_MINIMIZED:
		saved_state = state;
		break;
	}

	state = ws;
}

void Window::UpdateTitleButtons()
{
	const int size = 14;
	const int margin = 20;

	int y = bounds.y + GUI_TITLEBAR_HEIGHT / 2 - size / 2;
	int right = bounds.Right();

	bclose_bounds = Rect(right - margin, y, size, size);
	bmax_bounds = Rect(right - margin * 2, y, size, size);
	bmin_bounds = Rect(right - margin * 3, y, size, size);
}