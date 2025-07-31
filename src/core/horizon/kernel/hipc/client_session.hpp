#pragma once

#include "core/horizon/kernel/auto_object.hpp"

namespace hydra::horizon::kernel::hipc {

class Session;

class ClientSession : public AutoObject {
  public:
    ClientSession(const std::string_view debug_name = "Client session")
        : AutoObject(debug_name) {}

  private:
    Session* parent;

  public:
    GETTER_AND_SETTER(parent, GetParent, SetParent);
};

} // namespace hydra::horizon::kernel::hipc
