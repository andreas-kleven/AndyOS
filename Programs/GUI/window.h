#pragma once
#include "Elements/element.h"
#include "GUI/messages.h"
#include "guibase.h"
#include <AndyOS.h>
#include <andyos/drawing.h>
#include <sys/types.h>

namespace gui {
class Window : public GUIBase
{
  public:
    Color background;

    int id = 0;
    int width = 0;
    int height = 0;
    bool active = false;

    Window(const char *title, int width = 400, int height = 300, Color background = Color::White);
    ~Window();

    void Paint();

    void SetCapture(bool capture);

    void HandleMessage(const MESSAGE &message);

  protected:
    virtual void OnClose() {}
    virtual void OnResize() {}

  private:
    bool isClosed = false;

    void PaintElement(GUIBase *elem);
    void HoverElement(GUIBase *elem, int x, int y);

    Element *GetElementAt(int x, int y, GUIBase *parent);
};
} // namespace gui
