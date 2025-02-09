#pragma once

#include "horizon/services/service.hpp"

namespace Hydra::Horizon::Services::Time {

class SteadyClock : public ServiceBase {
  public:
    SteadyClock(Handle handle_) : ServiceBase(handle_) {}

    void Request(Kernel& kernel, Writer& writer, Writer& move_handles_writer,
                 u8* in_ptr) override;

  private:
};

} // namespace Hydra::Horizon::Services::Time
