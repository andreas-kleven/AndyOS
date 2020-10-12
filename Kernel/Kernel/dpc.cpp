#include <Kernel/dpc.h>
#include <debug.h>
#include <hal.h>
#include <sync.h>

namespace Dpc {
struct DPC_ENTRY
{
    DPC_HANDLER handler;
    void *arg;
};

Event queue_event;
Queue<DPC_ENTRY> queue;

void Insert(const DPC_ENTRY &entry)
{
    queue.Enqueue(entry);
    queue_event.Set();
}

void Create(DPC_HANDLER handler, void *arg)
{
    disable();
    DPC_ENTRY entry;
    entry.handler = handler;
    entry.arg = arg;
    Insert(entry);
    enable();
}

void Start()
{
    kprintf("Started dpc\n");

    while (true) {
        queue_event.Wait();
        disable();

        if (queue.Count() == 0) {
            queue_event.Clear();
            enable();
        } else {
            DPC_ENTRY entry = queue.Dequeue();
            enable();
            entry.handler(entry.arg);
        }
    }
}
} // namespace Dpc
