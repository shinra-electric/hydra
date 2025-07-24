#pragma once

#include "core/horizon/kernel/host_thread.hpp"
#include "core/horizon/services/service.hpp"

namespace hydra::horizon::kernel::hipc {
class ServerSession;
}

namespace hydra::horizon::services {

class Server {
  public:
    Server(IService* service);

    void RegisterSession(kernel::hipc::ServerSession* session);

  private:
    kernel::HostThread* thread;

    std::mutex sessions_mutex;
    std::vector<kernel::hipc::ServerSession*> sessions;

    void MainLoop(kernel::should_stop_fn_t should_stop);
    void ProcessRequests(kernel::hipc::ServerSession* session);
};

} // namespace hydra::horizon::services
