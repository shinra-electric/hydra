#include "core/horizon/kernel/hipc/server_session.hpp"

#include "core/horizon/kernel/thread.hpp"

namespace hydra::horizon::kernel::hipc {

ServerRequest ServerSession::Receive() {
    std::lock_guard<std::mutex> lock(mutex);
    ASSERT_DEBUG(!requests.empty(), Services, "No request available");
    return requests.front();
}

void ServerSession::Reply() {
    std::lock_guard<std::mutex> lock(mutex);
    const auto& request = requests.front();
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
