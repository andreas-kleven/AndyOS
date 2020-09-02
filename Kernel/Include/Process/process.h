#pragma once
#include <FS/file.h>
#include <Process/filetable.h>
#include <Process/thread.h>
#include <circbuf.h>
#include <memory.h>
#include <siginfo.h>
#include <sync.h>
#include <types.h>

#define PROC_MAX_MESSAGES 32
#define FILE_TABLE_SIZE   256
#define SIGNAL_TABLE_SIZE 32

#define SIG_DFL ((sig_t)0)
#define SIG_IGN ((sig_t)1)
#define SIG_ERR ((sig_t)-1)

#define SIGHUP  1
#define SIGINT  2
#define SIGQUIT 3
#define SIGKILL 9
#define SIGSTOP 17
#define SIGCONT 19
#define SIGCHLD 20

typedef void (*sig_t)(int signo);
typedef void MESSAGE_HANDLER(int id, int type, char *buf, int size);

struct sigaction
{
    sig_t sa_handler;
    sigset_t sa_mask;
    int sa_flags;
};

enum PROCESS_STATE
{
    PROCESS_STATE_RUNABLE,
    PROCESS_STATE_ZOMBIE,
    PROCESS_STATE_STOPPED
};

enum MESSAGE_TYPE
{
    MESSAGE_TYPE_MESSAGE,
    MESSAGE_TYPE_RESPONSE
};

struct MESSAGE
{
    MESSAGE_TYPE type;
    int id;
    int src_proc;
    int param;
    int size;
    char *data;

    MESSAGE()
    {
        id = 0;
        src_proc = 0;
        param = 0;
        size = 0;
        data = 0;
    }

    MESSAGE(MESSAGE_TYPE type, int id, int src_proc, int param, int size, char *data)
    {
        this->type = type;
        this->id = id;
        this->src_proc = src_proc;
        this->param = param;
        this->size = size;
        this->data = data;
    }

    MESSAGE(MESSAGE_TYPE type, int id, int src_proc, int param, int size)
    {
        this->type = type;
        this->id = id;
        this->src_proc = src_proc;
        this->param = param;
        this->size = size;
        this->data = new char[size];
    }
};

struct PROCESS
{
    PROCESS *next = 0;
    PROCESS *parent = 0;
    PROCESS *first_child = 0;
    PROCESS *next_sibling = 0;

    pid_t id = 0;
    gid_t gid = 0;
    pid_t sid = 0;
    ADDRESS_SPACE addr_space;
    PROCESS_STATE state = PROCESS_STATE_RUNABLE;
    siginfo_t siginfo;
    THREAD *main_thread = 0;
    size_t stack_ptr = 0;
    size_t heap_start = 0;
    size_t heap_end = 0;
    DENTRY *pwd = 0;

    Filetable *filetable;
    sig_t signal_table[SIGNAL_TABLE_SIZE];
    Mutex signal_mutex;

    MESSAGE_HANDLER *message_handler = 0;
    CircularBuffer<MESSAGE> *messages;

    PROCESS(ADDRESS_SPACE addr_space);
    ~PROCESS();
};

namespace ProcessManager {
pid_t AssignPid(PROCESS *proc);

PROCESS *AddProcess(PROCESS *proc, PROCESS *parent);
THREAD *CreateThread(PROCESS *proc, void (*entry)());
bool AddThread(PROCESS *proc, THREAD *thread);
bool RemoveThread(THREAD *thread);
bool StopThreads(PROCESS *proc);
bool FreeAllMemory(PROCESS *proc);
bool CloseFiles(PROCESS *proc);
void Terminate(PROCESS *proc);
void *AdjustHeap(PROCESS *proc, int increment);
void Exit(PROCESS *proc, int code);
int Chdir(PROCESS *process, const char *path);
int Fchdir(PROCESS *process, int fd);

PROCESS *GetProcess(pid_t id);
PROCESS *GetFirst();

PROCESS *Fork(PROCESS *proc);
PROCESS *Exec(const char *path);
PROCESS *Exec(const char *path, char const *argv[], char const *envp[]);
int Exec(PROCESS *proc, char const *path, char const *argv[], char const *envp[]);

sig_t SetSignalHandler(PROCESS *proc, int signo, sig_t handler);
int HandleSignal(pid_t sid, gid_t gid, int signo);
int HandleSignal(PROCESS *proc, int signo);
int FinishSignal(THREAD *thread);
}; // namespace ProcessManager
