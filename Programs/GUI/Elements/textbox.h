#pragma once
#include "element.h"
#include <string>

namespace gui {
class TextBox : public Element
{
  public:
    std::string text;

    TextBox();

    virtual void Paint();

    virtual void Focus();
    virtual void KeyPress(KEY_PACKET packet);
};
} // namespace gui