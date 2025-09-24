#include "core/horizon/kernel/host_thread.hpp"

#include "core/debugger/debugger_manager.hpp"

namespace hydra::horizon::kernel {

void HostThread::Run() {
    GET_CURRENT_PROCESS_DEBUGGER().RegisterThisThread(GetDebugName());
    run_callback([this]() {
        ProcessMessages();
        return GetState() == ThreadState::Stopping;
    });
    GET_CURRENT_PROCESS_DEBUGGER().UnregisterThisThread();
}

} // namespace hydra::horizon::kernel
