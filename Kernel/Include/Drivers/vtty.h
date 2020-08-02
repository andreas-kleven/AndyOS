#include <types.h>

namespace VTTY
{
    int SwitchTerminal(int id, bool force = false);
    void Start();
    void QueueInput(uint8 scan);
};
