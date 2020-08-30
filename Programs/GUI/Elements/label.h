#pragma once
#include "element.h"
#include <string>

namespace gui {
class Label : public Element
{
  public:
    std::string text;

    Label(const std::string &text);

    virtual void Paint();
};
} // namespace gui