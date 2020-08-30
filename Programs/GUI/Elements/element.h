#pragma once
#include "../guibase.h"
#include <andyos/drawing.h>

namespace gui {
class Element : public GUIBase
{
  public:
    Color foreground;
    Color background;

    bool isActive;

    Element(Rect bounds);
    Element(int x, int y, int width, int height);

    Rect GetGlobalBounds();
    GC CreateGC() const;

    virtual void Paint() {}
};
} // namespace gui