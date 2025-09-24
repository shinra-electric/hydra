#include "core/horizon/kernel/hipc/server_session.hpp"

#include "core/horizon/kernel/hipc/session.hpp"
#include "core/horizon/kernel/thread.hpp"

namespace hydra::horizon::kernel::hipc {

constexpr u64 MSG_BUFFER_MAX_SIZE = 0x2000; // TODO: what should this be?

ServerSession::~ServerSession() {
    // Resume the client thread if we still have an active request
    if (auto active_req = active_request)
        active_req->client_thread->CancelSync();

    parent->OnServerClose();
}

void ServerSession::OnClientClose() {
    std::lock_guard lock(mutex);
    client_open = false;

    // Signal the server that client has closed
    Signal();
}

void ServerSession::Receive(IThread* crnt_thread) {
    std::lock_guard lock(mutex);
    ASSERT_DEBUG(!requests.empty(), Kernel, "No requests");
    active_request = requests.front();
    requests.pop();

    // Copy the message to server TLS
    memcpy((void*)crnt_thread->GetTlsPtr(), (void*)active_request->ptr,
           MSG_BUFFER_MAX_SIZE);

    // Clear if no more requests
    if (requests.empty())
        Clear();
}

void ServerSession::Reply(uptr ptr) {
    std::lock_guard lock(mutex);

    // Copy the message to client TLS
    memcpy((void*)active_request->client_thread->GetTlsPtr(), (void*)ptr,
           MSG_BUFFER_MAX_SIZE);

    // Resume the client thread
    active_request->client_thread->Resume();

    active_request = std::nullopt;
}

void ServerSession::EnqueueRequest(Process* client_process,
                                   IThread* client_thread, uptr ptr) {
    std::lock_guard lock(mutex);
    requests.push({client_process, client_thread, ptr});

    // Signal the server to process the request
    Signal();
}

} // namespace hydra::horizon::kernel::hipc
