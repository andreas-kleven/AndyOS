#pragma once

namespace IRQ
{
    bool Install(int num, void(*handler)());
}