#pragma once

#include "core/horizon/kernel/synchronization_object.hpp"

namespace hydra::horizon::kernel::hipc {

class ServerSession;

class ServerPort : public SynchronizationObject {
  public:
    ServerPort(const std::string_view debug_name = "Server port")
        : SynchronizationObject(false, debug_name) {}

    // Server
    ServerSession* AcceptSession();

    // Client
    void ConnectSession(ServerSession* session);

  private:
    std::mutex mutex;
    std::queue<ServerSession*> incomming_sessions;
};

} // namespace hydra::horizon::kernel::hipc
