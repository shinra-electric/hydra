#pragma once

#include "horizon/const.hpp"

namespace Hydra::Horizon {
class Kernel;
}

namespace Hydra::Horizon::Services {

class ServiceBase {
  public:
    ServiceBase(Handle handle_) : handle{handle_} {}

    virtual void Request(Kernel& kernel, Writer& writer, u8* in_ptr) = 0;
    void Control(Kernel& kernel, Writer& writer, u8* in_ptr);

    // Getters
    // Handle GetHandle() const { return handle; }

  private:
    Handle handle;
};

} // namespace Hydra::Horizon::Services
