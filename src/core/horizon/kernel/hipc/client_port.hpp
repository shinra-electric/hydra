#pragma once

#include "core/horizon/kernel/synchronization_object.hpp"

namespace hydra::horizon::kernel::hipc {

class Port;
class ClientSession;

class ClientPort : public SynchronizationObject {
  public:
    ClientPort(const std::string_view debug_name = "Client port")
        : SynchronizationObject(true, debug_name) {}

    ClientSession* Connect();

  private:
    Port* parent;

  public:
    GETTER_AND_SETTER(parent, GetParent, SetParent);
};

} // namespace hydra::horizon::kernel::hipc
