#include "manager.h"
#include "GUI.h"
#include "GUI/messages.h"
#include "input.h"
#include "window.h"
#include <AndyOS.h>
#include <andyos/drawing.h>
#include <andyos/math.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

static uint32_t cursor_bitmap[8 * 14] = {
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
    0x00000000, 0x00000000, 0x00000000, 0xFF000000, 0xFF000000, 0xFF000000, 0x00000000, 0x00000000};

const int width = 1024;
const int height = 768;

const float sensitivity = 0.5f;
static bool has_rendered = false;

static Color col_taskbar;
static Color col_desktop_bg;

static GC gc;
static GC gc_background;
static GC gc_taskbar;
static GC gc_cursor;

static IMAGE *img_background = 0;

static int window_count = 0;
static Window *first_window = 0;
static Window *last_window = 0;

static Window *focused_window = 0;
static Window *active_window = 0;
static Window *hover_window = 0;

static float cursor_x = 0;
static float cursor_y = 0;
static bool cursor_moved = false;

static int cursor_left = false;
static int cursor_right = false;
static int cursor_middle = false;

static bool cursor_enabled;

static bool mouse_click_L;
static bool mouse_click_R;
static bool mouse_click_M;
static bool window_drag = false;

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

    LoadBackground("/files/sierra.bmp");
    CreateBackgroundGC();
    CreateCursorGC();

    Input::Init();

    pthread_t socket_thread;
    pthread_create(&socket_thread, 0, SocketLoop, 0);

    UpdateLoop();
}

void WindowManager::AddWindow(Window *wnd)
{
    wnd->next = 0;
    wnd->previous = 0;

    if (window_count) {
        wnd->previous = last_window;
        last_window->next = wnd;
    } else {
        first_window = wnd;
    }

    last_window = wnd;
    window_count += 1;
}

void WindowManager::CloseWindow(Window *wnd)
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

    window_count -= 1;

    gui::WINDOW_ACTION_MESSAGE msg(gui::WINDOW_ACTION_CLOSE);
    SendMessage(wnd, gui::MSGID_ACTION, &msg, sizeof(msg));

    delete wnd;
}

void WindowManager::MinimizeWindow(Window *wnd)
{
    SetFocusedWindow(0);
    wnd->Minimize();
}

void WindowManager::RestoreWindow(Window *wnd)
{
    wnd->Restore();
    SetFocusedWindow(wnd);
}

void WindowManager::LoadBackground(const char *filename)
{
    col_desktop_bg = Color::Magenta;
    img_background = IMAGE::Load(filename);
}

bool WindowManager::SendMessage(int sockfd, int id, int type, const void *data, int size)
{
    char buf[512];

    int msg_size = size + 12;

    if (msg_size > sizeof(buf)) {
        return false;
    }

    memcpy(&buf[0], &size, 4);
    memcpy(&buf[4], &type, 4);
    memcpy(&buf[8], &id, 4);
    memcpy(&buf[12], data, size);

    if (send(sockfd, buf, msg_size, 0) < 0)
        return false;

    return true;
}

bool WindowManager::SendMessage(Window *wnd, int type, const void *data, int size)
{
    return SendMessage(wnd->sockfd, wnd->id, type, data, size);
}

gui::MESSAGE WindowManager::MessageHandler(int sockfd, const gui::MESSAGE &msg)
{
    switch (msg.type) {
    case gui::REQID_CREATE_WINDOW: {
        gui::CREATE_WINDOW_REQUEST *request = (gui::CREATE_WINDOW_REQUEST *)msg.data;
        kprintf("Create window request: %s\n", request->title);

        int w = min(request->width, width);   // Hack
        int h = min(request->height, height); // Hack

        void *addr1;
        void *addr2;

        alloc_shared(request->pid, addr1, addr2, BYTES_TO_BLOCKS(width * height * 4));

        Window *wnd = new Window(request->pid, sockfd, request->title, w, h, (uint32_t *)addr1);

        gui::CREATE_WINDOW_RESPONSE response(wnd->id, (uint32_t *)addr2, wnd->gc.width,
                                             wnd->gc.height);
        SendMessage(wnd, gui::RESID_CREATE_WINDOW, &response, sizeof(response));

        WindowManager::AddWindow(wnd);
        WindowManager::SetFocusedWindow(wnd);
        WindowManager::SetActiveWindow(wnd);
    } break;

    case gui::REQID_PAINT: {
        gui::PAINT_REQUEST *request = (gui::PAINT_REQUEST *)msg.data;
        Window *wnd = GetWindow(msg.id);

        if (wnd) {
            wnd->dirty = true;
        }
    } break;

    case gui::REQID_SET_CAPTURE: {
        gui::SET_CAPTURE_REQUEST *request = (gui::SET_CAPTURE_REQUEST *)msg.data;
        Window *wnd = GetWindow(msg.id);

        wnd->capture = request->capture;
    } break;
    }

    return gui::MESSAGE(0);
}

void *WindowManager::SocketLoop(void *arg)
{
    int serverfd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (serverfd == -1) {
        perror("socket loop");
        return 0;
    }

    sockaddr_un unixaddr;
    unixaddr.sun_family = AF_UNIX;
    strcpy(unixaddr.sun_path, "/winman-sock");

    bind(serverfd, (sockaddr *)&unixaddr, sizeof(unixaddr));
    listen(serverfd, 10);

    while (true) {
        int clientfd = accept(serverfd, 0, 0, 0);
        kprintf("Client connected %d\n", clientfd);

        if (clientfd < 0) {
            perror("socket accept");
            sleep(1);
            continue;
        }

        pthread_create(0, 0, SocketHandler, (void *)clientfd);
    }
}

void *WindowManager::SocketHandler(void *arg)
{
    int sockfd = (int)arg;
    char buf[512];

    while (true) {
        int len = recv(sockfd, buf, sizeof(buf), 0);

        if (len < 0) {
            perror("socket recv");
            continue;
        }

        gui::MESSAGE msg = gui::MESSAGE(buf, len);
        gui::MESSAGE response = MessageHandler(sockfd, msg);

        if (response.type != gui::MSGID_NONE) {
            SendMessage(sockfd, response.id, response.type, response.data, response.size);

            if (response.copied)
                delete[] response.data;
        }
    }
}

void WindowManager::UpdateLoop()
{
    while (true) {
        cursor_enabled = !active_window || !active_window->capture;

        HandleMouseInput();
        HandleKeyInput();

        if (!ShouldRender()) {
            usleep(10000);
            continue;
        }

        if (active_window && active_window->fullscreen) {
            PaintWindows();
        } else {
            PaintBackground();
            PaintWindows();
            PaintTaskbar();
        }

        if (cursor_enabled) {
            PaintCursor();
        }

        gc.Draw();
        has_rendered = true;
    }
}

bool WindowManager::ShouldRender()
{
    if (!has_rendered)
        return true;

    if (cursor_moved)
        return true;

    Window *wnd = first_window;
    while (wnd) {
        if (wnd->dirty && wnd->state != WINDOW_STATE_MINIMIZED) {
            return true;
        }

        wnd = wnd->next;
    }

    return false;
}

void WindowManager::CreateCursorGC()
{
    for (int y = 0; y < 14; y++) {
        for (int x = 0; x < 8; x++) {
            uint32_t c = cursor_bitmap[x + y * 8];

            if (c != 0) {
                Color col = Color(c);
                gc_cursor.SetPixel(x, y, col);
            }
        }
    }
}

void WindowManager::CreateBackgroundGC()
{
    if (img_background) {
        gc_background.DrawImage(0, 0, img_background->width, img_background->height,
                                img_background);
        /*//Repeat

        int x = 0;
        while (x < width)
        {
                int  y = 0;
                while (y < height)
                {
                        gc_background.DrawImage(x, y, img_background->width,
        img_background->height, img_background); y += img_background->height;
                }

                x += img_background->width;
        }*/
    } else {
        gc_background.FillRect(0, 0, gc.width, gc.height, col_desktop_bg);
    }
}

void WindowManager::PaintBackground()
{
    gc_background.CopyTo(0, 0, gc_background.width, gc_background.height, gc, 0, 0);
}

void WindowManager::PaintWindows()
{
    if (window_count == 0)
        return;

    Window *start_window = last_window;
    Window *wnd = last_window;

    // Find first maximized window. Windows behind are not visible
    while (wnd) {
        if (wnd->state == WINDOW_STATE_MAXIMIZED) {
            start_window = wnd;
            break;
        }

        wnd = wnd->previous;
    }

    wnd = start_window;
    while (wnd) {
        if (wnd->state != WINDOW_STATE_MINIMIZED) {
            wnd->Paint(gc);
        }

        wnd = wnd->previous;
    }
}

void WindowManager::PaintTaskbar()
{
    int width = gc_taskbar.width;
    int height = gc_taskbar.height;
    int y = gc.height - gc_taskbar.height;

    gc_taskbar.FillRect(0, 0, gc_taskbar.width, gc_taskbar.height, col_taskbar);
    PaintTaskbarWindows();
    gc_taskbar.CopyTo(0, 0, gc_taskbar.width, gc_taskbar.height, gc, 0, y);
}

void WindowManager::PaintTaskbarWindows()
{
    const int margin = 10;
    const int size = gc_taskbar.height - 10;

    int x = margin;
    int y = (gc_taskbar.height - size) / 2;

    Window *wnd = last_window;

    while (wnd) {
        Color col = wnd->focused ? Color::LightGray : Color::Gray;
        gc_taskbar.FillRect(x, y, size, size, col);

        x += margin + size;

        if (x > width)
            break;

        wnd = wnd->previous;
    }
}

void WindowManager::PaintCursor()
{
    gc_cursor.CopyTo(0, 0, gc_cursor.width, gc_cursor.height, gc, (int)cursor_x, (int)cursor_y, 1);
}

void WindowManager::HandleMouseInput()
{
    bool left;
    bool right;
    bool middle;
    int dx, dy;

    Input::GetMouseButtons(left, right, middle);
    Input::GetMouseMovement(dx, dy);

    cursor_moved = false;

    if (cursor_enabled) {
        float prevx = cursor_x;
        float prevy = cursor_y;
        cursor_x = clamp(cursor_x + sensitivity * dx, 0.0f, (float)width);
        cursor_y = clamp(cursor_y - sensitivity * dy, 0.0f, (float)height);

        if (cursor_x != prevx || cursor_y != prevy)
            cursor_moved = true;
    }

    Window *wnd = GetWindowAtCursor();
    bool hovering_content = wnd && wnd->content_bounds.Contains(cursor_x, cursor_y);

    if (hovering_content) {
        if (left != cursor_left) {
            gui::KEY_INPUT_MESSAGE msg(KEY_LBUTTON, left);
            SendMessage(wnd, gui::MSGID_KEY_INPUT, &msg, sizeof(msg));
        }

        if (right != cursor_right) {
            gui::KEY_INPUT_MESSAGE msg(KEY_RBUTTON, right);
            SendMessage(wnd, gui::MSGID_KEY_INPUT, &msg, sizeof(msg));
        }

        if (middle != cursor_middle) {
            gui::KEY_INPUT_MESSAGE msg(KEY_MBUTTON, middle);
            SendMessage(wnd, gui::MSGID_KEY_INPUT, &msg, sizeof(msg));
        }
    }

    cursor_left = left;
    cursor_right = right;
    cursor_middle = middle;

    if (dx != 0 || dy != 0) {
        bool capture = active_window && active_window->capture;

        if (capture)
            wnd = active_window;

        if (hovering_content || capture) {
            int relx = cursor_x - wnd->bounds.x;
            int rely = cursor_y - wnd->bounds.y - GUI_TITLEBAR_HEIGHT;
            int _dx = (active_window == wnd) ? dx : 0;
            int _dy = (active_window == wnd) ? dy : 0;

            gui::MOUSE_INPUT_MESSAGE msg(relx, rely, _dx, _dy);
            SendMessage(wnd, gui::MSGID_MOUSE_INPUT, &msg, sizeof(msg));
        }
    }

    int time = get_ticks();

    if (left) {
        if (!mouse_click_L) {
            // Mouse down

            mouse_click_L_info.num_clicks += 1;

            if (time - mouse_click_L_info.click_time > DOUBLE_CLICK_TIME) {
                mouse_click_L_info.num_clicks = 0;
            }

            mouse_click_L = 1;
            mouse_click_L_info.click_time = time;
            mouse_click_L_info.click_x = cursor_x;
            mouse_click_L_info.click_y = cursor_y;

            mouse_click_L_info.window = wnd;

            if (wnd) {
                if (focused_window == 0 || wnd->id != focused_window->id) {
                    SetFocusedWindow(wnd);
                }

                if (wnd->content_bounds.Contains(cursor_x, cursor_y)) {
                    SetActiveWindow(wnd);
                } else if (wnd->titlebar_bounds.Contains(cursor_x, cursor_y)) {
                    if (wnd->bclose_bounds.Contains(cursor_x, cursor_y)) {
                        CloseWindow(wnd);
                    } else if (wnd->bmin_bounds.Contains(cursor_x, cursor_y)) {
                        MinimizeWindow(wnd);
                    } else if (wnd->bmax_bounds.Contains(cursor_x, cursor_y)) {
                        wnd->ToggleMaximized();
                    } else if (mouse_click_L_info.num_clicks == 1) {
                        wnd->ToggleMaximized();
                    }
                }
            }
        } else {
            // Drag

            int dx = cursor_x - mouse_click_L_info.click_x;
            int dy = cursor_y - mouse_click_L_info.click_y;

            if (wnd) {
                if (!window_drag && (dx != 0 || dy != 0)) {
                    int bw = 4;

                    window_drag_info.window = wnd;
                    window_drag_info.resize = false;
                    window_drag_info.bounds = wnd->bounds;

                    if (wnd->titlebar_bounds.Contains(mouse_click_L_info.click_x,
                                                      mouse_click_L_info.click_y)) {
                        window_drag = 1;
                    } /*else if (wnd->content_bounds.Contains(mouse_click_L_info.click_x,
                                                            mouse_click_L_info.click_y)) {
                        if (!Rect(wnd->bounds.x + bw, wnd->bounds.y + bw,
                                  wnd->bounds.width - bw * 2, wnd->bounds.height - bw * 2)
                                 .Contains(mouse_click_L_info.click_x,
                                           mouse_click_L_info.click_y)) {
                            window_drag = 1;
                            window_drag_info.resize = true;
                        }
                    }*/
                }
            }

            if (window_drag) {
                Window *dragwnd = window_drag_info.window;

                if (window_drag_info.resize) {
                    int w = window_drag_info.bounds.width + dx;
                    int h = window_drag_info.bounds.height + dy;
                    dragwnd->Resize(w, h);
                } else {
                    if (dragwnd->state == WINDOW_STATE_MAXIMIZED) {
                        // Set window state to normal when dragging maximized window

                        float ratio = cursor_x / (float)width;
                        int nx = cursor_x - dragwnd->normal_bounds.width * ratio;

                        dragwnd->normal_bounds.x = nx;
                        dragwnd->normal_bounds.y = 0;
                        dragwnd->SetState(WINDOW_STATE_NORMAL);

                        window_drag_info.bounds.x = dragwnd->bounds.x;
                        window_drag_info.bounds.y = dragwnd->bounds.y;
                    }

                    int x = window_drag_info.bounds.x + dx;
                    int y = window_drag_info.bounds.y + dy;
                    window_drag_info.window->Move(x, y);
                }
            }
        }
    } else {
        if (mouse_click_L) {
            // Mouse up

            MOUSE_CLICK_INFO &info = mouse_click_L_info;

            mouse_click_L = 0;
            info.rel_time = time;
            info.rel_x = cursor_x;
            info.rel_y = cursor_y;

            if (window_drag) {
                window_drag = 0;

                Window *dragwnd = window_drag_info.window;

                if (dragwnd) {
                    if (dragwnd->bounds.y == 0)
                        dragwnd->SetState(WINDOW_STATE_MAXIMIZED);
                }
            } else {
            }
        }
    }
}

void WindowManager::HandleKeyInput()
{
    KEYCODE code;
    bool pressed;

    while (Input::GetKeyboardInput(code, pressed)) {
        gui::InputManager::HandleKey(code, pressed);

        if (active_window) {
            gui::KEY_INPUT_MESSAGE msg(code, pressed);
            SendMessage(active_window, gui::MSGID_KEY_INPUT, &msg, sizeof(msg));
        }

        // alt+tab
        if (gui::InputManager::GetKeyDown(KEY_LALT) && gui::InputManager::GetKeyDown(KEY_TAB)) {
            SetActiveWindow(0);
        }

        // alt+enter
        if (focused_window) {
            if (gui::InputManager::GetKeyDown(KEY_LALT) &&
                gui::InputManager::GetKeyDown(KEY_RETURN)) {
                focused_window->SetFullscreen(!focused_window->fullscreen);
            }
        }
    }
}

Window *WindowManager::GetWindowAtCursor()
{
    Window *wnd = first_window;
    while (wnd) {
        if (wnd->state != WINDOW_STATE_MINIMIZED) {
            if (wnd->bounds.Contains(cursor_x, cursor_y)) {
                return wnd;
            }
        }

        wnd = wnd->next;
    }

    return 0;
}

void WindowManager::SetFocusedWindow(Window *new_focused)
{
    focused_window = new_focused;
    SetActiveWindow(0);

    if (focused_window && !focused_window->focused) {
        if (focused_window != first_window) {
            if (focused_window == last_window && focused_window->previous)
                last_window = focused_window->previous;

            // Disconnect
            if (focused_window->previous)
                focused_window->previous->next = focused_window->next;
            if (focused_window->next)
                focused_window->next->previous = focused_window->previous;

            // Insert first
            focused_window->next = first_window;
            focused_window->previous = 0;

            if (first_window->next)
                first_window->next->previous = first_window;
            first_window->previous = focused_window;
            first_window = focused_window;
        }

        focused_window->SetFocus(1);
    }

    Window *wnd = first_window;
    while (wnd) {
        if (wnd != focused_window && wnd->focused) {
            wnd->SetFocus(0);
        }

        wnd = wnd->next;
    }
}

void WindowManager::SetActiveWindow(Window *wnd)
{
    if (wnd == active_window)
        return;

    if (active_window) {
        gui::DEACTIVATED_MESSAGE msg = gui::DEACTIVATED_MESSAGE();
        SendMessage(active_window, gui::MSGID_DEACTIVATED, &msg, sizeof(msg));
    }

    active_window = wnd;

    if (active_window) {
        gui::ACTIVATED_MESSAGE msg = gui::ACTIVATED_MESSAGE();
        SendMessage(active_window, gui::MSGID_ACTIVATED, &msg, sizeof(msg));
    }
}

Window *WindowManager::GetWindow(int id)
{
    Window *wnd = first_window;

    while (wnd) {
        if (wnd->id == id)
            return wnd;

        wnd = wnd->next;
    }

    return 0;
}
