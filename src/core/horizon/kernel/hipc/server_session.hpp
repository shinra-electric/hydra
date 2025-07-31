#pragma once

#include "core/horizon/kernel/hipc/const.hpp"
#include "core/horizon/kernel/synchronization_object.hpp"

namespace hydra::horizon::kernel {
class Process;
}

namespace hydra::horizon::kernel::hipc {

struct SessionRequest {
    Process* client_process;
    IThread* client_thread;
    uptr ptr;
};

class ServerSession : public SynchronizationObject {
  public:
    ServerSession(const std::string_view debug_name = "Server session")
        : SynchronizationObject(false, debug_name) {}
    ~ServerSession() override;

    // Server
    void Receive(IThread* crnt_thread);
    void Reply(uptr ptr);

    // HACK
    kernel::Process* GetActiveRequestClientProcess() {
        std::lock_guard lock(mutex);
        return active_request->client_process;
    }

    // Client
    void EnqueueRequest(Process* client_process, IThread* client_thread,
                        uptr ptr);

  private:
    std::mutex mutex;
    std::queue<SessionRequest> requests;
    std::optional<SessionRequest> active_request;
};

} // namespace hydra::horizon::kernel::hipc
