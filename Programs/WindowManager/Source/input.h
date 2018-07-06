#pragma once

namespace Input
{
    void Init();

    void GetMouseButtons(bool& left, bool& right, bool& middle);
    void GetMouseMovement(int& x, int& y);
}