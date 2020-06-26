#include "guibase.h"

namespace gui
{
    GUIBase::GUIBase()
    {
        this->parent = 0;
        this->isHovering = false;
    }

    void GUIBase::AddChild(GUIBase* child)
    {
        child->parent = this;
        elements.push_back(child);
    }
}