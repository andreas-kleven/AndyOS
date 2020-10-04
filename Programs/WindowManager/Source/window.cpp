#include "window.h"
#include "GUI.h"
#include "manager.h"
#include <andyos/math.h>
#include <string.h>
#include <sys/types.h>

static int new_id = 1;

Window::Window(int proc_id, int sockfd, char *title, int width, int height, uint32_t *framebuffer)
{
    color_background = Color(0, 0.5, 0.5);
    color_foreground = Color::White;
    color_title = Color::Black;

    this->id = new_id++;
    this->proc_id = proc_id;
    this->sockfd = sockfd;

    this->title = new char[strlen(title) + 1];
    strcpy(this->title, title);

    state = WINDOW_STATE_NORMAL;
    focused = false;
    dirty = true;

    Move((id - 1) * 300, (id - 1) * 300);
    gc = GC(content_bounds.width, content_bounds.height, framebuffer);
    Resize(width, height, false);
}

void Window::Paint(GC &main_gc)
{
    // if (dirty)

    if (fullscreen) {
        gc.CopyTo(0, 0, gc.width, gc.height, main_gc, bounds.x, bounds.y);
    } else {
        const int bw = GUI_WINDOW_BORDER_WIDTH;

        gc.CopyTo(0, 0, gc.width, gc.height, main_gc, bounds.x, bounds.y + titlebar_bounds.height);

        Color _titlebar = focused ? color_background : Color::White;
        Color _border = focused ? color_background : Color::Black;

        main_gc.FillRect(bounds.x, bounds.y, bounds.width, titlebar_bounds.height,
                         _titlebar);                                      // Title bar
        main_gc.DrawText(bounds.x + 4, bounds.y + 4, title, color_title); // Title

        main_gc.FillRect(bclose_bounds, Color::Red);
        main_gc.FillRect(bmin_bounds, Color::Yellow);
        main_gc.FillRect(bmax_bounds, Color::Green);

        main_gc.DrawRect(bounds.x - bw, bounds.y - bw, bounds.width + bw * 2,
                         bounds.height + bw * 2, bw, _border); // Window border
    }

    dirty = false;
}

void Window::Move(int x, int y)
{
    y = max(y, 0);

    bounds.x = x;
    bounds.y = y;
    content_bounds.x = x;
    content_bounds.y = y + titlebar_bounds.height;
    titlebar_bounds.x = x;
    titlebar_bounds.y = y;

    UpdateTitleButtons();
}

void Window::Resize(int w, int h, bool send)
{
    w = max(w, 100);
    h = max(h, GUI_TITLEBAR_HEIGHT + 30);

    titlebar_bounds.width = w;
    titlebar_bounds.height = fullscreen ? 0 : GUI_TITLEBAR_HEIGHT;

    bounds.width = w;
    bounds.height = h;
    content_bounds.width = w;
    content_bounds.height = fullscreen ? h : (h - titlebar_bounds.height);

    gc.Resize(content_bounds.width, content_bounds.height);

    UpdateTitleButtons();

    if (send) {
        gui::RESIZE_MESSAGE msg(gc.width, gc.height);
        WindowManager::SendMessage(this, gui::MSGID_RESIZE, &msg, sizeof(msg));
    }
}

void Window::Close()
{}

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

    if (ws == WINDOW_STATE_NORMAL) {
        SetState(WINDOW_STATE_MAXIMIZED);
    } else if (ws == WINDOW_STATE_MAXIMIZED) {
        SetState(WINDOW_STATE_NORMAL);
    }
}

void Window::SetFocus(bool focus)
{
    this->focused = focus;
}

void Window::SetFullscreen(bool fullscreen)
{
    if (fullscreen == this->fullscreen)
        return;

    this->fullscreen = fullscreen;

    if (this->fullscreen)
        SetState(WINDOW_STATE_MAXIMIZED, true);
    else
        SetState(WINDOW_STATE_NORMAL, true);
}

void Window::SetState(WINDOW_STATE ws)
{
    return SetState(ws, false);
}

void Window::SetState(WINDOW_STATE ws, bool force)
{
    if (ws == state && !force)
        return;

    if (state == WINDOW_STATE_NORMAL)
        normal_bounds = bounds;

    switch (ws) {
    case WINDOW_STATE_NORMAL:
        Move(normal_bounds.x, normal_bounds.y);
        Resize(normal_bounds.width, normal_bounds.height);
        break;

    case WINDOW_STATE_MAXIMIZED:
        Move(0, 0);
        Resize(1024, fullscreen ? 768 : (768 - GUI_TASKBAR_HEIGHT));
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

    int y = bounds.y + titlebar_bounds.height / 2 - size / 2;
    int right = bounds.Right();

    bclose_bounds = Rect(right - margin, y, size, size);
    bmax_bounds = Rect(right - margin * 2, y, size, size);
    bmin_bounds = Rect(right - margin * 3, y, size, size);
}
