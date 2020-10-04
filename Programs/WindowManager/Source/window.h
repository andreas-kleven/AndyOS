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

    int id;
    int sockfd;
    int proc_id;
    char *title;
    bool capture;
    bool fullscreen;
    WINDOW_STATE state;

    Rect bounds;
    Rect content_bounds;
    Rect titlebar_bounds;
    Rect normal_bounds;
    Rect bclose_bounds;
    Rect bmin_bounds;
    Rect bmax_bounds;

    bool focused;
    bool dirty;

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
