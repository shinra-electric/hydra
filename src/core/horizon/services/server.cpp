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
    kernel::hipc::ServerSession* reply_target_session = nullptr;
    while (true) {
        // Wait for incoming requests
        // TODO: don't create a new vector
        std::vector<kernel::SynchronizationObject*> sync_objs;
        sync_objs.reserve(sessions.size());
        sync_objs.insert(sync_objs.end(), sessions.begin(), sessions.end());

        i32 signalled_index;
        const auto res = KERNEL_INSTANCE.ReplyAndReceive(
            thread, sync_objs, reply_target_session, kernel::INFINITE_TIMEOUT,
            signalled_index);
        ASSERT_DEBUG(res == RESULT_SUCCESS, Services,
                     "Failed to wait for synchronization: 0x{:08x}", res);

        // Process incoming requests
        auto session = sessions[signalled_index];
        session->GetService()->HandleRequest(
            *this, session->GetActiveRequestClientProcess(),
            thread->GetTlsPtr());

        // Check for exit
        if (should_stop())
            break;

        // Set the reply target
        reply_target_session = session;
    }
}

} // namespace hydra::horizon::services
