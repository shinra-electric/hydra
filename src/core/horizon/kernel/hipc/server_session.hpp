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

typedef std::function<void()> request_finished_callback_fn_t;
typedef std::function<void(Process*, uptr)> request_handler_fn_t;

class ServerSession : public SynchronizationObject {
  public:
    ServerSession(services::IService* service_,
                  const std::string_view debug_name = "ServerSession")
        : SynchronizationObject(false, debug_name), service{service_} {}

    void PushRequest(Process* caller_process, uptr ptr,
                     request_finished_callback_fn_t finished_callback);
    void HandleAllRequests(request_handler_fn_t request_handler);

  private:
    services::IService* service;

    struct ServerRequest {
        Process* caller_process;
        uptr ptr;
        request_finished_callback_fn_t finished_callback;
    };

    std::mutex mutex;
    std::vector<ServerRequest> requests;

  public:
    GETTER(service, GetService);
};

} // namespace hydra::horizon::kernel::hipc
