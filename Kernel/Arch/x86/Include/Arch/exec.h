#include <Process/thread.h>

namespace Exec::Arch {
int SetupMain(THREAD *thread, char const *argv[], char const *envp[]);
int SetupThreadMain(THREAD *thread, const void (*start_routine)(), const void *arg);
} // namespace Exec::Arch
