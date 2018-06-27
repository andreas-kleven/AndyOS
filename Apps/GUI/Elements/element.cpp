#include "element.h"

namespace gui
{
    Element::Element(Rect bounds)
    {
        this->bounds = bounds;
    }

    Element::Element(int x, int y, int width, int height)
        : Element(Rect(x, y, width, height))
    {
    }

    GC Element::CreateGC() const
    {
        return GC(parent->gc, bounds.x, bounds.y, bounds.width, bounds.height);
    }
}