#pragma once

#include "core/horizon/kernel/auto_object.hpp"

namespace hydra::horizon::kernel::hipc {

class ServerPort;
class ClientPort;

class Port : public AutoObject {
  public:
    Port(ServerPort* server_side_, ClientPort* client_side_,
         const std::string_view debug_name = "Port");

  private:
    ServerPort* server_side;
    ClientPort* client_side;

  public:
    GETTER(server_side, GetServerSide);
};

} // namespace hydra::horizon::kernel::hipc
