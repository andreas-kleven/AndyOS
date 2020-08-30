#include <types.h>
#include <Drivers/vt100.h>

namespace VTTY {
int SwitchTerminal(int id, bool force = false);
Vt100Driver *CurrentTerminal();
void Start();
void QueueInput(uint8 scan);
}; // namespace VTTY
