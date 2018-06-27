#include "guibase.h"

namespace gui
{
    void GUIBase::AddChild(GUIBase* child)
    {
        child->parent = this;
        elements.Add(child);
    }
}