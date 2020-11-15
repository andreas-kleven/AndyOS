#pragma once
#include <types.h>

namespace FPU {

void SetTS();
void ClearTS();
void CleanState(void *state);
STATUS Init();

} // namespace FPU
