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
    template <typename Key>
    void
    RegisterServiceToPort(kernel::hipc::ServiceManager<Key>& service_manager,
                          const Key& port_name, IService* service) {
        std::string debug_name;
        if constexpr (std::is_same_v<Key, std::string>)
            debug_name = port_name;
        else
            debug_name = u64_to_str(port_name);

        // Session
        auto server_session = new kernel::hipc::ServerSession(
            service, fmt::format("\"{}\" server session", debug_name));
        auto client_session = new kernel::hipc::ClientSession(
            fmt::format("\"{}\" client session", debug_name));
        new kernel::hipc::Session(server_session, client_session,
                                  fmt::format("\"{}\" session", debug_name));

        // Register server side
        RegisterSession(server_session);

        // Register client side
        service_manager.RegisterPort(port_name, client_session);
    }

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
