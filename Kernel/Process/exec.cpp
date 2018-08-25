#include "process.h"
#include "hal.h"
#include "elf.h"
#include "string.h"
#include "Kernel/task.h"
#include "scheduler.h"
#include "debug.h"

namespace ProcessManager
{
    PROCESS* Exec(const char* path)
    {
		disable();

        ADDRESS_SPACE old_space = VMem::GetAddressSpace();
		ADDRESS_SPACE addr_space;

        if (!VMem::CreateAddressSpace(&addr_space))
            return 0;

		VMem::SwapAddressSpace(addr_space);

		PROCESS* proc = new PROCESS(PROCESS_USER, addr_space);
        size_t entry = ELF::Load(path, proc);

        if (entry)
        {
		    if (ProcessManager::CreateThread(proc, (void(*)())entry))
            {
		        VMem::SwapAddressSpace(old_space);
                enable();
		        return AddProcess(proc);
            }
        }

        //Todo: cleanup
        VMem::SwapAddressSpace(old_space);
        enable();
        return 0;
    }

    bool Exec(PROCESS* proc, char const *path, char const *argv[], char const *envp[])
    {
        StopThreads(proc, false);
        FreeMemory(proc);

        size_t entry = ELF::Load(path, proc);

        if (entry)
        {
            THREAD* thread = ProcessManager::CreateThread(proc, (void(*)())entry);

		    if (thread)
            {
                Scheduler::InsertThread(thread);
                Task::Switch(); //execution stops here
            }
        }

        return false;
    }
}