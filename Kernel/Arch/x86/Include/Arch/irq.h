#pragma once

namespace IRQ::Arch {
bool Install(int num, void (*handler)());
}