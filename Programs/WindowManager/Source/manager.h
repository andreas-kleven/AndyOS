#pragma once
#include "GUI/messages.h"
#include "window.h"
struct MOUSE_CLICK_INFO
{
    Window *window = 0;

    int click_time = 0;
    int click_x = 0;
    int click_y = 0;

    int rel_time = 0;
    int rel_x = 0;
    int rel_y = 0;

    int num_clicks = 0;
};

struct WINDOW_DRAG_INFO
{
    Window *window;
    bool resize;
    Rect bounds;
};

class WindowManager
{
  public:
    static void Start();

    static bool SendMessage(int sockfd, int id, int type, const void *data, int size);
    static bool SendMessage(Window *wnd, int type, const void *data, int size);

    static void AddWindow(Window *wnd);
    static void CloseWindow(Window *wnd);
    static void MinimizeWindow(Window *wnd);
    static void RestoreWindow(Window *wnd);

    static void LoadBackground(const char *filename);

  private:
    static gui::MESSAGE MessageHandler(int sockfd, const gui::MESSAGE &msg);
    static void *SocketLoop(void *arg);
    static void *SocketHandler(void *arg);
    static void UpdateLoop();
    static bool ShouldRender();

    static void CreateCursorGC();
    static void CreateBackgroundGC();

    static void PaintBackground();
    static void PaintWindows();
    static void PaintTaskbar();
    static void PaintTaskbarWindows();
    static void PaintCursor();

    static void HandleMouseInput();
    static void HandleKeyInput();

    static Window *GetWindowAtCursor();
    static void SetFocusedWindow(Window *new_focused);
    static void SetActiveWindow(Window *wnd);

    static Window *GetWindow(int id);
};
