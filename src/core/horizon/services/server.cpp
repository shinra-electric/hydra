#include "core/horizon/services/server.hpp"

#include "core/horizon/kernel/hipc/service_manager.hpp"
#include "core/horizon/kernel/kernel.hpp"

namespace hydra::horizon::services {

void Server::Start() {
    // TODO: clean up as well

    // Process
    // TODO: process

    // Thread
    thread = new kernel::HostThread(
        nullptr, 0x20,
        [this](kernel::should_stop_fn_t should_stop) { MainLoop(should_stop); },
        "Service server thread");
    thread->Start();
}

void Server::RegisterSession(kernel::hipc::ServerSession* session) {
    sessions.push_back(session);
}

void Server::MainLoop(kernel::should_stop_fn_t should_stop) {
    while (true) {
        // Wait for incoming requests
        // TODO: don't create a new vector
        std::vector<kernel::SynchronizationObject*> sync_objs;
        sync_objs.reserve(sessions.size());
        sync_objs.insert(sync_objs.end(), sessions.begin(), sessions.end());

        u64 signalled_index;
        const auto res = KERNEL_INSTANCE.WaitSynchronization(
            thread, sync_objs, kernel::INFINITE_TIMEOUT, signalled_index);
        ASSERT_DEBUG(res == RESULT_SUCCESS, Services,
                     "Failed to wait for synchronization: 0x{:08x}", res);

        // Process incoming requests
        ProcessRequests(sessions[signalled_index]);

        // Check for exit
        if (should_stop())
            break;
    }
}

void Server::ProcessRequests(kernel::hipc::ServerSession* session) {
    session->HandleAllRequests(
        [this, session](kernel::Process* caller_process, uptr ptr) {
            session->GetService()->HandleRequest(*this, caller_process, ptr);
        });
}

} // namespace hydra::horizon::services
