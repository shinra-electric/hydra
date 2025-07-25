#include "core/horizon/kernel/hipc/server_session.hpp"

namespace hydra::horizon::kernel::hipc {

void ServerSession::PushRequest(
    Process* caller_process, uptr ptr,
    request_finished_callback_fn_t finished_callback) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        requests.push_back({caller_process, ptr, finished_callback});
    }

    Signal();
}

void ServerSession::HandleAllRequests(request_handler_fn_t request_handler) {
    Clear();

    {
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& request : requests) {
            request_handler(request.caller_process, request.ptr);
            request.finished_callback();
        }
        requests.clear();
    }
}

} // namespace hydra::horizon::kernel::hipc
