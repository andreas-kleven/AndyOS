#include <Arch/exec.h>
#include <Process/process.h>
#include <Process/elf.h>
#include <Process/scheduler.h>
#include <Kernel/task.h>
#include <FS/vfs.h>
#include <hal.h>
#include <string.h>
#include <errno.h>
#include <debug.h>

namespace ProcessManager
{
    struct PROCESS_START_INFO
    {
        char const **argv;
        char const **envp;
        int arg_count;
        int env_count;
        char *argenv_ptr;
        size_t argenv_size;
    };

    char *CopyTable(char *dest, char const *strings[], int &count)
    {
        int length = 0;
        const char **ptr = strings;

        while (*ptr)
        {
            count++;
            length += strlen(*ptr) + 1;
            ptr++;
        }

        size_t table_size = (count + 1) * 4;

        char **table = (char **)dest;
        memset(table, 0, table_size + length);
        dest += table_size;

        for (int i = 0; i < count; i++)
        {
            strcpy(dest, strings[i]);
            table[i] = dest;
            dest += strlen(strings[i]) + 1;
        }

        return dest;
    }

    int CopyStrings(char const *argv[], char const *envp[], PROCESS_START_INFO *psi)
    {
        if (!argv || !envp)
            return -1;

        int length = 16;

        const char **ptr1 = argv;
        const char **ptr2 = envp;

        while (*ptr1)
        {
            length += strlen(*ptr1) + 5;
            ptr1++;
        }

        while (*ptr2)
        {
            length += strlen(*ptr2) + 5;
            ptr2++;
        }

        psi->arg_count = 0;
        psi->env_count = 0;

        psi->argenv_size = length;
        psi->argenv_ptr = new char[length];
        psi->argv = (char const **)psi->argenv_ptr;
        psi->envp = (char const **)CopyTable((char *)psi->argv, argv, psi->arg_count);
        CopyTable((char *)psi->envp, envp, psi->env_count);

        return 0;
    }

    int MoveStringsUser(PROCESS_START_INFO *psi)
    {
        void *argenv_ptr = VMem::UserAlloc(BYTES_TO_BLOCKS(psi->argenv_size));

        if (!argenv_ptr)
            return -1;

        memcpy(argenv_ptr, psi->argenv_ptr, psi->argenv_size);

        size_t offset = (size_t)argenv_ptr - (size_t)psi->argenv_ptr;

        const char **new_argv = (const char **)((size_t)psi->argv + offset);
        const char **new_envp = (const char **)((size_t)psi->envp + offset);

        for (int i = 0; i < psi->arg_count; i++)
            new_argv[i] += offset;

        for (int i = 0; i < psi->env_count; i++)
            new_envp[i] += offset;

        delete psi->argenv_ptr;

        psi->argv = new_argv;
        psi->envp = new_envp;

        return 0;
    }

    int SetupMain(THREAD *thread, PROCESS_START_INFO *psi)
    {
        if (MoveStringsUser(psi))
            return -1;

        return Exec::Arch::SetupMain(thread, psi->argv, psi->envp);
    }

    PROCESS *Exec(const char *path)
    {
        ADDRESS_SPACE old_space = VMem::GetAddressSpace();
        ADDRESS_SPACE addr_space;

        if (!VMem::CreateAddressSpace(addr_space))
            return 0;

        VMem::SwapAddressSpace(addr_space);

        PROCESS *proc = new PROCESS(PROCESS_USER, addr_space);
        size_t entry = ELF::Load(path, proc);

        if (entry)
        {
            THREAD *thread = ProcessManager::CreateThread(proc, (void (*)())entry);

            if (thread)
            {
                const char *argv[] = {0};
                const char *envp[] = {0};

                PROCESS_START_INFO psi;

                if (CopyStrings(argv, envp, &psi))
                    return 0;

                if (SetupMain(thread, &psi))
                    return 0;

                VMem::SwapAddressSpace(old_space);
                return AddProcess(proc, NULL);
            }
        }

        //Todo: cleanup
        VMem::SwapAddressSpace(old_space);
        return 0;
    }

    int Exec(PROCESS *proc, char const *path, char const *argv[], char const *envp[])
    {
        if (!VFS::GetDentry(path))
            return -ENOENT;

        int count = 0;

        char path_copy[strlen(path) + 1];
        strcpy(path_copy, path);

        PROCESS_START_INFO psi;

        if (CopyStrings(argv, envp, &psi))
            return -1;

        StopThreads(proc);
        FreeAllMemory(proc);

        size_t entry = ELF::Load(path_copy, proc);

        if (!entry)
            return -1;

        THREAD *thread = ProcessManager::CreateThread(proc, (void (*)())entry);

        if (thread)
        {
            if (SetupMain(thread, &psi))
                return -1;

            Scheduler::InsertThread(thread);
        }

        return 0;
    }
} // namespace ProcessManager
