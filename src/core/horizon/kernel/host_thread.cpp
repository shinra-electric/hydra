#include "core/horizon/kernel/host_thread.hpp"

#include "core/debugger/debugger.hpp"

namespace hydra::horizon::kernel {

void HostThread::Run() {
    DEBUGGER_INSTANCE.RegisterThisThread(GetDebugName());
    run_callback([this]() {
        ProcessMessages();
        return GetState() == ThreadState::Stopping;
    });
    DEBUGGER_INSTANCE.UnregisterThisThread();
}

} // namespace hydra::horizon::kernel
