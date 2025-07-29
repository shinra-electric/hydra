#pragma once

#include "core/horizon/kernel/hipc/client_session.hpp"
#include "core/horizon/kernel/hipc/server_session.hpp"
#include "core/horizon/kernel/hipc/service_manager.hpp"
#include "core/horizon/kernel/hipc/session.hpp"
#include "core/horizon/kernel/host_thread.hpp"
#include "core/horizon/services/service.hpp"

namespace hydra::horizon::kernel::hipc {
class ServerSession;
} // namespace hydra::horizon::kernel::hipc

namespace hydra::horizon::services {

class Server {
  public:
    void Start();

    void RegisterSession(kernel::hipc::ServerSession* session);

  private:
    kernel::HostThread* thread;

    std::vector<kernel::hipc::ServerSession*> sessions;

    void MainLoop(kernel::should_stop_fn_t should_stop);

  public:
    GETTER(thread, GetThread);
};

} // namespace hydra::horizon::services
