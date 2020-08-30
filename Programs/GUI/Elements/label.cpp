#include "label.h"

namespace gui {
Label::Label(const std::string &text) : Element(0, 0, 256, 16)
{
    this->text = text;
    foreground = Color::Black;
    background = Color::White;
}

void Label::Paint()
{
    GC gc = CreateGC();

    gc.Clear(background);
    gc.DrawText(0, 0, text.c_str(), foreground, background);
}
} // namespace gui
