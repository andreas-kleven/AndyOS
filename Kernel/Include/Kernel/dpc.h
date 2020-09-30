#pragma once

typedef void (*DPC_HANDLER)(void *);

namespace Dpc {
void Create(DPC_HANDLER handler, void *arg);
void Start();
} // namespace Dpc
