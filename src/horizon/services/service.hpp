#pragma once

#include "horizon/const.hpp"

namespace Hydra::Horizon {
class Kernel;
}

namespace Hydra::Horizon::Services {

struct Writers {
    Writer writer;
    Writer objects_writer;
    Writer move_handles_writer;
    Writer copy_handles_writer;
};

class ServiceBase {
  public:
    virtual void Request(Writers& writers, u8* in_ptr,
                         std::function<void(ServiceBase*)> add_service) = 0;
    void Control(Kernel& kernel, Writer& writer, u8* in_ptr);

    // Getters
    // Handle GetHandle() const { return handle; }

    // Setters
    void SetHandle(Handle handle_) { handle = handle_; }

  private:
    Handle handle;
};

} // namespace Hydra::Horizon::Services
