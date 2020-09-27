#include <Kernel/task.h>
#include <Process/dispatcher.h>
#include <Process/process.h>
#include <Process/scheduler.h>
#include <string.h>

namespace ProcessManager {
bool CopyThreads(PROCESS *proc, PROCESS *newproc)
{
    THREAD *current_thread = Dispatcher::CurrentThread();
    THREAD *thread = proc->main_thread;

    while (thread) {
        if (thread->state != THREAD_STATE_TERMINATED) {
            THREAD *copy = Task::CopyThread(thread);

            if (!copy)
                return false;

            if (!AddThread(newproc, copy))
                return false;

            if (thread == current_thread) {
                copy->state = THREAD_STATE_READY;
                Task::SetThreadReturn(copy, 0);
            }
        }

        thread = thread->proc_next;
    }

    return true;
}

PROCESS *Fork(PROCESS *proc)
{
    Scheduler::Disable();
    debug_print("Fork %d\n", proc->id);

    VMem::SwapAddressSpace(proc->addr_space);
    ADDRESS_SPACE space;

    if (!VMem::CopyAddressSpace(space)) {
        Scheduler::Enable();
        return 0;
    }

    VMem::SwapAddressSpace(space);

    PROCESS *newproc = new PROCESS(space);
    AssignPid(newproc);

    newproc->stack_ptr = proc->stack_ptr;
    newproc->heap_start = proc->heap_start;
    newproc->heap_end = proc->heap_end;
    newproc->pwd = proc->pwd;
    newproc->pwd->refs += 1;
    newproc->sid = proc->sid;
    newproc->gid = proc->gid;

    newproc->filetable = proc->filetable->Clone();
    memcpy(newproc->signal_table, proc->signal_table, SIGNAL_TABLE_SIZE);

    THREAD *current_thread = Scheduler::CurrentThread();

    if (!CopyThreads(proc, newproc)) {
        // Todo: cleanup
        Scheduler::Enable();
        return 0;
    }

    VMem::SwapAddressSpace(proc->addr_space);

    if (!AddProcess(newproc, proc)) {
        Scheduler::Enable();
        return 0;
    }

    debug_print("Fork complete %d\n", newproc->id);
    Scheduler::Enable();
    return newproc;
}
} // namespace ProcessManager
