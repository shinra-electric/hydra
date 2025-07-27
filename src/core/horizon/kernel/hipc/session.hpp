#pragma once

#include "core/horizon/kernel/auto_object.hpp"

namespace hydra::horizon::kernel {
class Process;
}

namespace hydra::horizon::kernel::hipc {

class ServerSession;
class ClientSession;

class Session : public AutoObject {
  public:
    Session(ServerSession* server_side_, ClientSession* client_side_,
            const std::string_view debug_name = "Session");

  private:
    ServerSession* server_side;
    ClientSession* client_side;

  public:
    GETTER(server_side, GetServerSide);
};

} // namespace hydra::horizon::kernel::hipc
