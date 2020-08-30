#include <Kernel/dpc.h>
#include <debug.h>
#include <hal.h>
#include <sync.h>

namespace Dpc {
struct DPC_ENTRY
{
    void (*handler)();
};

Event queue_event;
Queue<DPC_ENTRY> queue;

void Insert(const DPC_ENTRY &entry)
{
    queue.Enqueue(entry);
    queue_event.Set();
}

void Create(void (*handler)())
{
    DPC_ENTRY entry;
    entry.handler = handler;
    Insert(entry);
}

void Start()
{
    debug_print("Started dpc\n");

    while (true) {
        queue_event.Wait();
        disable();

        if (queue.Count() == 0) {
            queue_event.Clear();
            enable();
        } else {
            DPC_ENTRY entry = queue.Dequeue();
            enable();
            entry.handler();
        }
    }
}
} // namespace Dpc
