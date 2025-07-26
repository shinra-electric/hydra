#pragma once

#include "core/horizon/kernel/hipc/const.hpp"
#include "core/horizon/kernel/synchronization_object.hpp"

namespace hydra::horizon::kernel {
class Process;
}

namespace hydra::horizon::services {
class IService;
}

namespace hydra::horizon::kernel::hipc {

struct ServerRequest {
    Process* client_process;
    uptr ptr;
    IThread* client_thread;
};

class ServerSession : public SynchronizationObject {
  public:
    ServerSession(services::IService* service_,
                  const std::string_view debug_name = "ServerSession")
        : SynchronizationObject(false, debug_name), service{service_} {}

    // Server
    void Receive(IThread* crnt_thread);
    void Reply(uptr ptr);

    // HACK
    kernel::Process* GetActiveRequestClientProcess() {
        std::lock_guard<std::mutex> lock(mutex);
        return requests.front().client_process;
    }

    // Client
    void EnqueueRequest(Process* client_process, uptr ptr,
                        IThread* client_thread);

  private:
    services::IService* service;

    std::mutex mutex;
    std::queue<ServerRequest> requests;

  public:
    GETTER(service, GetService);
};

} // namespace hydra::horizon::kernel::hipc
