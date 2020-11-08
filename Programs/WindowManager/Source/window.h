#pragma once
#include <andyos/drawing.h>

#define GUI_WINDOW_BORDER_WIDTH 1
#define GUI_TITLEBAR_HEIGHT     22

enum WINDOW_STATE
{
    WINDOW_STATE_NORMAL,
    WINDOW_STATE_MAXIMIZED,
    WINDOW_STATE_MINIMIZED
};

class Window
{
  public:
    Color color_background;
    Color color_foreground;
    Color color_title;

    int id = 0;
    int sockfd = 0;
    int proc_id = 0;
    char *title = 0;
    bool capture = false;
    bool focused = false;
    bool dirty = true;
    bool fullscreen = false;
    WINDOW_STATE state = WINDOW_STATE_NORMAL;

    Rect bounds;
    Rect content_bounds;
    Rect titlebar_bounds;
    Rect normal_bounds;
    Rect bclose_bounds;
    Rect bmin_bounds;
    Rect bmax_bounds;

    GC gc;

    Window *next;
    Window *previous;

    Window(int proc_id, int sockfd, char *title, int width, int height, uint32_t *framebuffer);

    void Paint(GC &main_gc);
    void Move(int x, int y);
    void Resize(int w, int h, bool send = true);

    void Close();
    void Minimize();
    void Restore();
    void ToggleMaximized();

    void SetFocus(bool focus);
    void SetFullscreen(bool fullscreen);
    void SetState(WINDOW_STATE state);
    void SetState(WINDOW_STATE state, bool force);

  private:
    WINDOW_STATE saved_state;

    void UpdateTitleButtons();
};
