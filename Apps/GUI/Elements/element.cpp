#include "element.h"

namespace gui
{
    Element::Element(Rect bounds)
    {
        this->bounds = bounds;
        this->isActive = false;
    }

    Element::Element(int x, int y, int width, int height)
        : Element(Rect(x, y, width, height))
    {
    }

    Rect Element::GetGlobalBounds()
    {
        GUIBase* p = parent;
        Rect b = bounds;

        while (p)
        {
            b.x += p->bounds.x;
            b.y += p->bounds.y;
            p = p->parent;
        }

        return b;
    }

    GC Element::CreateGC() const
    {
        return GC(parent->gc, bounds.x, bounds.y, bounds.width, bounds.height);
    }
}