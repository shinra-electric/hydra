#pragma once

#include "core/debugger/debugger.hpp"

#define DEBUGGER_MANAGER_INSTANCE debugger::DebuggerManager::GetInstance()
#define GET_CURRENT_PROCESS_DEBUGGER()                                         \
    DEBUGGER_MANAGER_INSTANCE.GetDebuggerForCurrentProcess()

namespace hydra::debugger {

class DebuggerManager {
  public:
    static DebuggerManager& GetInstance();

    DebuggerManager();
    ~DebuggerManager();

    void AttachDebugger(hydra::horizon::kernel::Process* process,
                        const std::string_view name);
    void DetachDebugger(hydra::horizon::kernel::Process* process);

    Debugger& GetDebugger(hydra::horizon::kernel::Process* process);
    Debugger& GetDebuggerForCurrentProcess();

    // API
    void Lock() { mutex.lock(); }
    void Unlock() { mutex.unlock(); }

    usize GetDebuggerCount() const { return debuggers.size() + 1; }
    Debugger& GetDebugger(const u32 index) {
        if (index == 0)
            return hydra_debugger;

        // TODO: not the best way to index into a map
        auto it = debuggers.begin();
        std::advance(it, index - 1);
        return it->second;
    }

  private:
    std::mutex mutex;
    Debugger hydra_debugger;
    std::map<hydra::horizon::kernel::Process*, Debugger> debuggers;
};

} // namespace hydra::debugger
