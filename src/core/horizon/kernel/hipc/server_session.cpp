#include "core/horizon/kernel/hipc/server_session.hpp"

#include "core/horizon/kernel/thread.hpp"

namespace hydra::horizon::kernel::hipc {

constexpr u64 MSG_BUFFER_MAX_SIZE = 0x2000; // TODO: what should this be?

void ServerSession::Receive(IThread* crnt_thread) {
    std::lock_guard<std::mutex> lock(mutex);
    ASSERT_DEBUG(!requests.empty(), Services, "No request available");
    const auto& request = requests.front();

    // Copy the message to server TLS
    memcpy((void*)crnt_thread->GetTlsPtr(), (void*)request.ptr,
           MSG_BUFFER_MAX_SIZE);
}

void ServerSession::Reply(uptr ptr) {
    std::lock_guard<std::mutex> lock(mutex);
    const auto& request = requests.front();

    // Copy the message to client TLS
    memcpy((void*)request.client_thread->GetTlsPtr(), (void*)ptr,
           MSG_BUFFER_MAX_SIZE);

    // Resume the client thread
    request.client_thread->Resume();
    requests.pop();
    if (requests.empty())
        Clear();
}

void ServerSession::EnqueueRequest(Process* client_process, uptr ptr,
                                   IThread* client_thread) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        requests.push({client_process, ptr, client_thread});
    }

    Signal();
}

} // namespace hydra::horizon::kernel::hipc
